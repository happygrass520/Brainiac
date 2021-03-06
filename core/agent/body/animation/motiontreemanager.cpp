// Brainiac is a free and open source tool for the creation of crowd simulation

// Copyright (C) 2013  Dirk Fuchs dirkfux@googlemail.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "motiontreemanager.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include "core/agent/body/animation/motiontree.h"
#include "core/agent/body/animation/motiontreeaction.h"
#include "core/agent/body/animation/motiontreetransition.h"
#include "gui/Animation/motiontreeeditoritem.h"
#include "core/agent/agentmanager.h"
#include "core/brainiaclogger.h"

MotionTreeManager::MotionTreeManager(AgentManager *agentManager, QObject *parent) :
    QObject(parent),m_agentManager(agentManager)
{
    createTrees();
}

void MotionTreeManager::addDefaultMotionVariable(QString variable)
{
    if(!m_defaultMotionVariableNames.contains(variable))
        m_defaultMotionVariableNames.append(variable);
}

quint32 MotionTreeManager::addTrigger(QString triggerName, quint32 id)
{
    if(id) {
        if(m_triggers.contains(id)) {
            qWarning() << __PRETTY_FUNCTION__ << "id already exists";
            id=m_latchIdGenerator.getNewId();
        }
        m_latchIdGenerator.registerId(id);
    } else {
        id=m_latchIdGenerator.getNewId();
    }
    m_triggers.insert(id,triggerName);
    return id;
}

void MotionTreeManager::deleteDefaultMotionVariable(QString variable)
{
    int index=m_defaultMotionVariableNames.indexOf(variable);
    if(index!=-1) {
        m_defaultMotionVariableNames.removeAt(index);
    }
}

void MotionTreeManager::createTrees()
{
    for(quint32 i=0;i<NUM_OF_TREE_TRACKS;i++) {
        MotionTree *mt=new MotionTree(i,this);
        m_motionTrees.insert(i,mt);
    }
}

void MotionTreeManager::loadConfig(QXmlStreamReader *stream)
{
    Q_ASSERT(stream->isStartElement() && stream->name() == BrainiacGlobals::XmlMotionTreesTag);
    while(stream->readNextStartElement()) {
        if(stream->name() == BrainiacGlobals::XmlMotionTreeTag) {
            QXmlStreamAttributes attribs = stream->attributes();
            int treeId=attribs.value(BrainiacGlobals::XmlIdAttrib).toInt();
            MotionTree *tree=getMotionTrees().value(treeId);
            tree->setTreeDefaultAction(attribs.value(BrainiacGlobals::XmlTreeDefaultActionAttrib).toString());
            while(stream->readNextStartElement()) {
                if(stream->name()==BrainiacGlobals::XmlTreeActionTag) {
                    QXmlStreamAttributes attribs = stream->attributes();
                    QString actionName=tree->addAction(attribs.value(BrainiacGlobals::XmlNameAttrib).toString());
                    MotionTreeAction *action=tree->getActions().value(actionName,0);
                    if(action) {
                        action->getEditorItem()->setPos(attribs.value(BrainiacGlobals::XmlEditorXPosAttrib).toString().toInt(),attribs.value(BrainiacGlobals::XmlEditorYPosAttrib).toString().toInt());
                        QString triggers=attribs.value(BrainiacGlobals::XmlTriggersAttrib).toString();
                        foreach(QString triggerId, triggers.split(" ")) {
                            action->addTriggerId(triggerId.toUInt());
                        }
                    } else {
                        qCWarning(bAnimation) << __PRETTY_FUNCTION__ << "unknown Action " << actionName;
                    }
                } else if(stream->name()==BrainiacGlobals::XmlTreeTransitionTag) {
                    QXmlStreamAttributes attribs = stream->attributes();
                    QString transName=tree->addTransition(attribs.value(BrainiacGlobals::XmlNameAttrib).toString());
                    MotionTreeTransition *trans=tree->getTransitions().value(transName,0);
                    if(trans) {
                        trans->setColorId(attribs.value(BrainiacGlobals::XmlColorAttrib).toString().toInt());
                        trans->getEditorItem()->setPos(attribs.value(BrainiacGlobals::XmlEditorXPosAttrib).toString().toInt(),attribs.value(BrainiacGlobals::XmlEditorYPosAttrib).toString().toInt());
                    } else {
                        qCWarning(bAnimation) << __PRETTY_FUNCTION__ << "unknown Transition " << transName;
                    }
                } else if(stream->name()==BrainiacGlobals::XmlTreeActionTransitionConnectionTag) {
                    QXmlStreamAttributes attribs = stream->attributes();
                    tree->connectActionWithTransition(attribs.value(BrainiacGlobals::XmlTreeActionAttrib).toString(),attribs.value(BrainiacGlobals::XmlTreeTransitionAttrib).toString());
                } else if(stream->name()==BrainiacGlobals::XmlTreeTransitionActionConnectionTag) {
                    QXmlStreamAttributes attribs = stream->attributes();
                    tree->connectTransitionWithAction(attribs.value(BrainiacGlobals::XmlTreeTransitionAttrib).toString(),attribs.value(BrainiacGlobals::XmlTreeActionAttrib).toString());
                }
                stream->skipCurrentElement();
            }
            //stream->skipCurrentElement();
        } else if(stream->name()==BrainiacGlobals::XmlTriggerTag) {
            QXmlStreamAttributes attribs = stream->attributes();
            addTrigger(attribs.value(BrainiacGlobals::XmlNameAttrib).toString(),attribs.value(BrainiacGlobals::XmlIdAttrib).toUInt());
            stream->skipCurrentElement();
        } else {
            stream->skipCurrentElement();
        }
            //stream->skipCurrentElement(); // XmlMotionTreeTag
        //stream->skipCurrentElement();
    }

    //stream->skipCurrentElement(); // XmlMotionTreesTag
}

void MotionTreeManager::saveConfig(QXmlStreamWriter *stream)
{
    stream->writeStartElement(BrainiacGlobals::XmlMotionTreesTag); // MotionTrees
    // Triggers
    QHashIterator<quint32, QString> itTriggers(m_triggers);
    while(itTriggers.hasNext()) {
        itTriggers.next();
        stream->writeStartElement(BrainiacGlobals::XmlTriggerTag);
        stream->writeAttribute(BrainiacGlobals::XmlIdAttrib,QString::number(itTriggers.key()));
        stream->writeAttribute(BrainiacGlobals::XmlNameAttrib,itTriggers.value());
        stream->writeEndElement(); // XmlTriggerTag
    }

    for(unsigned int i=0; i<MotionTreeManager::NUM_OF_TREE_TRACKS; i++) {
        MotionTree *tree=getMotionTrees().value(i);
        stream->writeStartElement(BrainiacGlobals::XmlMotionTreeTag); // MotionTree
        stream->writeAttribute(BrainiacGlobals::XmlTreeDefaultActionAttrib,tree->getDefaultActionName());
        stream->writeAttribute(BrainiacGlobals::XmlIdAttrib,QString::number(i));

        // Actions
        foreach(MotionTreeAction *action, tree->getActions())  {
            stream->writeStartElement(BrainiacGlobals::XmlTreeActionTag);
            stream->writeAttribute(BrainiacGlobals::XmlNameAttrib,tree->getActionName(action));
            stream->writeAttribute(BrainiacGlobals::XmlEditorXPosAttrib,QString::number(action->getEditorItem()->pos().x()));
            stream->writeAttribute(BrainiacGlobals::XmlEditorYPosAttrib,QString::number(action->getEditorItem()->pos().y()));
            QString triggerIds;
            foreach(quint32 id, action->getTriggerIds()) {
                triggerIds.append(QString::number(id)).append(" ");
            }
            triggerIds=triggerIds.trimmed();
            stream->writeAttribute(BrainiacGlobals::XmlTriggersAttrib,triggerIds);

            stream->writeEndElement(); // XmlTreeActionTag
        }
        // Transitions
        foreach(MotionTreeTransition *trans, tree->getTransitions())  {
            stream->writeStartElement(BrainiacGlobals::XmlTreeTransitionTag);
            stream->writeAttribute(BrainiacGlobals::XmlNameAttrib,tree->getTransitionName(trans));
            stream->writeAttribute(BrainiacGlobals::XmlEditorXPosAttrib,QString::number(trans->getEditorItem()->pos().x()));
            stream->writeAttribute(BrainiacGlobals::XmlEditorYPosAttrib,QString::number(trans->getEditorItem()->pos().y()));
            stream->writeAttribute(BrainiacGlobals::XmlColorAttrib,QString::number(trans->getColorId()));
            stream->writeEndElement(); // XmlTreeTransitionTag
        }

        QHashIterator<MotionTreeAction *, MotionTreeTransition *> itAT=tree->getActionTransitionConnections();
        while (itAT.hasNext()) {
            itAT.next();
            stream->writeStartElement(BrainiacGlobals::XmlTreeActionTransitionConnectionTag);
            stream->writeAttribute(BrainiacGlobals::XmlTreeActionAttrib,tree->getActionName(itAT.key()));
            stream->writeAttribute(BrainiacGlobals::XmlTreeTransitionAttrib,tree->getTransitionName(itAT.value()));
            stream->writeEndElement(); // ActionTransition
        }

        QHashIterator<MotionTreeTransition *, MotionTreeAction *> itTA=tree->getTransitionActionConnections();
        while (itTA.hasNext()) {
            itTA.next();
            stream->writeStartElement(BrainiacGlobals::XmlTreeTransitionActionConnectionTag);
            stream->writeAttribute(BrainiacGlobals::XmlTreeTransitionAttrib,tree->getTransitionName(itTA.key()));
            stream->writeAttribute(BrainiacGlobals::XmlTreeActionAttrib,tree->getActionName(itTA.value()));
            stream->writeEndElement(); // TransitionAction
        }

        stream->writeEndElement(); // MotionTree
    }

    stream->writeEndElement(); // MotionTrees
}

quint32 MotionTreeManager::NUM_OF_TREE_TRACKS=8;

MotionTreeManager::~MotionTreeManager()
{
    foreach(MotionTree *mt, m_motionTrees) {
        mt->deleteLater();
    }
}
