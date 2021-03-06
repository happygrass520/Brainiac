// Brainiac is a free and open source tool for the creation of crowd simulation

// Copyright (C) 2012  Dirk Fuchs dirkfux@googlemail.com

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

#include "bodymanager.h"
#include "core/group/group.h"
#include "core/agent/agentmanager.h"
#include "core/agent/agent.h"
#include "core/agent/body/body.h"
#include "core/agent/body/bodysegment.h"
#include "core/agent/body/segmentbox.h"
#include "core/agent/body/segmenttube.h"
#include "core/agent/body/segmentsphere.h"
#include "gui/bodyeditor/bodyeditor.h"
#include <QObject>

BodyManager::BodyManager(AgentManager *manager)
{
    m_agentManager=manager;
    resetNewSegmentProperties();
    m_bodyEditor=new BodyEditor(m_agentManager->getScene(),m_agentManager); //!< @todo Fix this! Ugly!
    m_rootSegmentRotTransOrder=BrainiacGlobals::AgentDefaultRotTrans;
}

void BodyManager::addSegmentToAgents(quint32 id)
{
    foreach(Agent *agent,m_agentManager->getAllManagedAgents() ) {
        osg::ref_ptr<BodySegment> bs=new BodySegment(agent->getBody(), m_segments.value(id));
        agent->getBody()->addBodySegment(bs.get(),bs.get()->getParentId());
    }
}

void BodyManager::addSegmentsToAgent(Agent *agent, quint32 startSegmentId)
{
//    qDebug()<< __PRETTY_FUNCTION__ << "Adding segment id" << startSegmentId;
    osg::ref_ptr<BodySegment> bs=new BodySegment(agent->getBody(), m_segments.value(startSegmentId));
    agent->getBody()->addBodySegment(bs.get(),bs.get()->getParentId());
    foreach(quint32 childId,getSegmentChildIds(startSegmentId)) {
//        qDebug() << __PRETTY_FUNCTION__ << "Add child " << childId;
        this->addSegmentsToAgent(agent,childId);
    }
}

void BodyManager::applyBodyOnAgent(Agent *agent)
{
    if(m_segments.count()==0) {
        //qDebug() << __PRETTY_FUNCTION__ << "No segments found!";
        return;
    }
    Segment *rootSeg=0;

    foreach(Segment *seg,m_segments) {
        if(seg->isRootSegment()) {
            rootSeg=seg;
            break;
        }
    }

    if(rootSeg) {
        this->addSegmentsToAgent(agent,rootSeg->getId());
    } else {
        qDebug() << __PRETTY_FUNCTION__ << "No root segment found for agent " << agent->getId() << m_agentManager->getGroup()->getName();
    }

}

bool BodyManager::createNewSegment(bool processRootId)
{
    // first check, if everything is valid

    // If this should be a root node, check if we have already a root node
    if(m_newSegment.isRootSegment()) {
        foreach(Segment *seg, m_segments) {
            if(seg->isRootSegment()) {
                qDebug() << __PRETTY_FUNCTION__ << "Error: Trying to add a second root segment";
                resetNewSegmentProperties();
                return false;
            }
        }
    }
    // check, if the new segments id is already in use
    if(m_segments.contains(m_newSegment.getId())) {
        qDebug() << __PRETTY_FUNCTION__ << "Error: Id is already in use";
        resetNewSegmentProperties();
        return false;
    }
    if(m_newSegment.getId()==0 && !processRootId) {
        m_newSegment.setId(m_agentManager->getSegmentIdGenerator().getNewId());
    } else {
        m_agentManager->getSegmentIdGenerator().registerId(m_newSegment.getId());
    }
    SegmentShape *newSegment=0;

    switch(m_newSegment.getType()) {
    case BrainiacGlobals::BOXSEGMENT:
        newSegment=new SegmentBox(m_newSegment);
        break;
    case BrainiacGlobals::TUBESEGMENT:
        newSegment=new SegmentTube(m_newSegment);
        break;
    case BrainiacGlobals::SPHERESEGMENT:
        newSegment=new SegmentSphere(m_newSegment);
        break;
    default:
        qCritical() << __PRETTY_FUNCTION__ << "This never sould happen";
    }

    //SegmentShape *newSegment=new SegmentShape(m_newSegment);
    m_segments.insert(newSegment->getId(),newSegment);
    addSegmentToAgents(newSegment->getId());
    resetNewSegmentProperties();
    m_bodyEditor->addSegment(newSegment->getId());
    QObject::connect(newSegment,SIGNAL(updated()),m_agentManager,SLOT(segmentChanged()));
    return true;
}

QList<quint32> BodyManager::getChildIds(quint32 id) const
{
    QList<quint32> children;
    foreach(SegmentShape *seg, m_segments) {
        if(seg->getParentId()==id) {
            children.append(seg->getId());
        }
    }
    return children;
}

Segment BodyManager::getSegment(quint32 id) const
{
    if(m_segments.contains(id)) {
        return Segment(*m_segments.value(id));
    }
    else
        return Segment();
}

QList<quint32> BodyManager::getTraversedSegmentIds() const
{
    QList<quint32> ids;
    if(!hasRootSegment()) {
        return ids;
    }
    quint32 rootId=getRootSegment().getId();
//    ids.append(rootId);
    getTraversedSegmentIdsRec(&ids,rootId);
    return ids;
}

void BodyManager::getTraversedSegmentIdsRec(QList<quint32> *list,quint32 startId) const
{
    list->append(startId);
    foreach(quint32 childId, getSegmentChildIds(startId)) {
        getTraversedSegmentIdsRec(list,childId);
    }
}

Segment BodyManager::getRootSegment() const
{
    foreach(SegmentShape *seg,m_segments) {
        if(seg->isRootSegment()) {
            return Segment(*seg);
        }
    }
    qDebug() << __PRETTY_FUNCTION__ << "No root segment for agent " << m_agentManager->getName();
    return Segment();
}

QList<quint32> BodyManager::getSegmentChildIds(quint32 id) const
{
    SegmentShape *s=m_segments.value(id);
    QList<quint32> childIds;
    foreach(SegmentShape* segShape,m_segments) {
        if(segShape->getParentId()==s->getId()) {
            childIds.append(segShape->getId());
            //qDebug() << __PRETTY_FUNCTION__ << "Child found: "<< segShape->getId() << "#" << childIds.count();
        }
    }
    return childIds;
}

bool BodyManager::hasChildren(quint32 id) const
{
    return getChildIds(id).empty();
}

bool BodyManager::hasRootSegment() const {
    foreach(SegmentShape* segShape,m_segments) {
        if(segShape->isRootSegment())
            return true;
    }
    return false;
}

void BodyManager::resetNewSegmentProperties()
{
    m_newSegment.setColor(1.0f);
    m_newSegment.setColorInherited(false);
    m_newSegment.setRestRotation(QVector3D());
    m_newSegment.setRotation(QVector3D());
    m_newSegment.setTranslation(QVector3D());
    m_newSegment.setRestTranslation(QVector3D());
    m_newSegment.setSize(QVector3D());
    m_newSegment.setName("New Segment");
    m_newSegment.setId(0);
    m_newSegment.setParentId(0);
}


void BodyManager::setNewSegmentBySegment(Segment &other)
{
    m_newSegment.copyFromOther(other);
}

void BodyManager::setNewSegmentRotationTranslationOrder(QList<BrainiacGlobals::RotTrans> order)
{
    m_newSegment.setRotationTranslationOrder(order);
}

void BodyManager::setSegmentDiameter(quint32 id, qreal diameter)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setDiameter(diameter);
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}

void BodyManager::setSegmentLength(quint32 id, qreal length)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setLength(length);
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}


void BodyManager::setSegmentRestRotation(quint32 id, qreal x, qreal y, qreal z)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setRestRotation(QVector3D(x,y,z));
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}

void BodyManager::setSegmentRotation(quint32 id, qreal x, qreal y, qreal z)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setRotation(QVector3D(x,y,z));
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}

void BodyManager::setSegmentRestTranslation(quint32 id, qreal x, qreal y, qreal z)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setRestTranslation(QVector3D(x,y,z));
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}

void BodyManager::setSegmentSize(quint32 id, qreal x, qreal y, qreal z)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setSize(QVector3D(x,y,z));
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}

void BodyManager::setSegmentTranslation(quint32 id, qreal x, qreal y, qreal z)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setTranslation(QVector3D(x,y,z));
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}

qreal BodyManager::getSegmentDiameter(quint32 id) const
{
    if(m_segments.contains(id)) {
        return m_segments.value(id)->getDiameter();
    }
    else {
        qCritical() << __PRETTY_FUNCTION__ << "No Segment with id" << id;
    }
    return 0;
}

int BodyManager::getSegmentLevel(quint32 id) const
{
    int level=0;
    SegmentShape *s=m_segments.value(id,0);
    if(!s) {
        return -1;
    }
    quint32 parentId=s->getParentId();
    while(s->getParentId() != 0) {
        s=m_segments.value(parentId);
        parentId=s->getParentId();
        level++;
    }
    return level;
}

qreal BodyManager::getSegmentLength(quint32 id) const
{
    if(m_segments.contains(id)) {
        return m_segments.value(id)->getLength();
    }
    else {
        qCritical() << __PRETTY_FUNCTION__ << "No Segment with id" << id;
    }
    return 0;
}

QVector3D BodyManager::getSegmentRestRotation(quint32 id) const
{
    if(m_segments.contains(id)) {
        return m_segments.value(id)->getRestRotation();
    }
    else {
        qCritical() << __PRETTY_FUNCTION__ << "No Segment with id" << id;
    }
    return QVector3D();
}

QVector3D BodyManager::getSegmentRotation(quint32 id) const
{
    if(m_segments.contains(id)) {
        return m_segments.value(id)->getRotation();
    }
    else {
        qCritical() << __PRETTY_FUNCTION__ << "No Segment with id" << id;
    }
    return QVector3D();
}

QVector3D BodyManager::getSegmentRestTranslation(quint32 id) const
{
    if(m_segments.contains(id)) {
        return m_segments.value(id)->getRestTranslation();
    }
    else {
        qCritical() << __PRETTY_FUNCTION__ << "No Segment with id" << id;
    }
    return QVector3D();
}

QVector3D BodyManager::getSegmentSize(quint32 id) const
{
    if(m_segments.contains(id)) {
        return m_segments.value(id)->getSize();
    }
    else {
        qCritical() << __PRETTY_FUNCTION__ << "No Segment with id" << id;
    }
    return QVector3D();
}

QVector3D BodyManager::getSegmentTranslation(quint32 id) const
{
    if(m_segments.contains(id)) {
        return m_segments.value(id)->getTranslation();
    }
    else {
        qCritical() << __PRETTY_FUNCTION__ << "No Segment with id" << id;
    }
    return QVector3D();
}

void BodyManager::setSegmentRotationTranslationOrder(quint32 id, QList<BrainiacGlobals::RotTrans> order)
{
    if(m_segments.contains(id))
        m_segments.value(id)->setRotationTranslationOrder(order);
    else
        qCritical() << __PRETTY_FUNCTION__ << "No segment with id "<< id;
}

void BodyManager::dDumpBody()
{
    foreach(Segment *seg,m_segments) {
        qDebug() << seg->getId() << seg->getName() << seg->getParentId() << seg->getRestTranslation() << seg->getTranslation() << seg->getRotation();
    }
}

BodyManager::~BodyManager()
{
    foreach(Segment *s,m_segments) {
        s->deleteLater();
    }
    m_segments.clear();
    m_bodyEditor->deleteLater();
}
