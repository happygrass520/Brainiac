#include "agentmanager.h"
#include "core/agent/agent.h"
#include "core/scene.h"
#include "core/agent/agent.h"
#include "core/agent/body/body.h"
#include "core/agent/body/sphere.h"
#include "core/agent/brain/brain.h"
#include "core/agent/brain/fuzzybase.h"
#include "core/agent/brain/output.h"
#include "core/agent/channel.h"
#include "core/group/group.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QDebug>
#include <QVector3D>

AgentManager::AgentManager(Scene *scene, Group *group)
{
    m_scene=scene;
    m_group=group;
    m_id=0;
    m_masterAgent=new Agent(m_scene,0); // Id 0 is ok, its just a master agent
}

void AgentManager::addSphereFromConfig(QXmlStreamReader *reader, quint32 id, QString name, quint32 parent)
{
    QVector3D *translation;
    QVector3D *rotation;
    qreal radius;
    qreal color;
    bool colorInherited;

    while(reader->readNextStartElement()) {
        if(reader->name()=="Translation") {
            qDebug() << "Sphere Translation";
            QXmlStreamAttributes attribs = reader->attributes();
            translation= new QVector3D();
            translation->setX(attribs.value("x").toString().toDouble());
            translation->setY(attribs.value("y").toString().toDouble());
            translation->setZ(attribs.value("z").toString().toDouble());
            reader->skipCurrentElement();
        } else if(reader->name()=="Rotation") {
            qDebug() << "Sphere Rotation";
            QXmlStreamAttributes attribs = reader->attributes();
            rotation= new QVector3D();
            rotation->setX(attribs.value("x").toString().toDouble());
            rotation->setY(attribs.value("y").toString().toDouble());
            rotation->setZ(attribs.value("z").toString().toDouble());
            reader->skipCurrentElement();
        } else if(reader->name()=="Radius") {
            qDebug() << "Sphere Radius";
            QXmlStreamAttributes attribs = reader->attributes();
            radius=attribs.value("r").toString().toDouble();
            reader->skipCurrentElement();
        }  else if(reader->name()=="Color") {
            QXmlStreamAttributes attribs = reader->attributes();
            color=attribs.value("value").toString().toDouble();
            colorInherited=attribs.value("inherited").toString().compare("true",Qt::CaseInsensitive)==0;
            reader->skipCurrentElement();
            qDebug() << "Sphere Color "<< color << colorInherited;
        }
    }
    Segment *parentSeg=m_masterAgent->getBody()->getSegment(parent);

    Segment *seg=new Sphere(id, m_masterAgent->getBody(),name,rotation,translation,radius,0);
    seg->setColorInherited(colorInherited);
    seg->getColor()->init(color);
    if( parentSeg ) {
        qDebug() << "Segment with id"<< id << "has parent";
        seg->setParentId(parentSeg->getId());
    }
    m_masterAgent->getBody()->addSegment(seg);
    //reader->skipCurrentElement();

}

void AgentManager::addOutputFuzz(quint32 id, QString name, QString channel, quint32 editorX, quint32 editorY)
{
    m_masterAgent->addOutputFuzz(id, name, channel, editorX, editorY);
}

/** \brief clones an agent

                this function clones an agent from this manager´s master agent

        \param  id the id of the new agent
        \return pointer to new agent instance
**/
Agent* AgentManager::cloneAgent(quint32 id)
{
    Agent *agent=new Agent(m_scene,id);
    foreach(Segment *seg, m_masterAgent->getBody()->getSegments()) {
        if(seg->getType()==Segment::SPHERE) {
            Sphere *origSphere=(Sphere*)seg;
            QVector3D *rot=new QVector3D(origSphere->getRestRotation()->x(),origSphere->getRestRotation()->y(),origSphere->getRestRotation()->z());
            QVector3D *trans=new QVector3D(origSphere->getRestTranslation()->x(),origSphere->getRestTranslation()->y(),origSphere->getRestTranslation()->z());
            qreal color=origSphere->getColor()->getValue();
            bool colorInherited=origSphere->isColorInherited();
            Sphere *newSphere=new Sphere(origSphere->getId(),agent->getBody(),origSphere->getName(),rot,trans,origSphere->getRestRadius());
            newSphere->setParentId(origSphere->getParentId());
            newSphere->getColor()->init(color);
            newSphere->setColorInherited(colorInherited);
            agent->getBody()->addSegment(newSphere);
        } else {
            qDebug() <<  __PRETTY_FUNCTION__ << "missing segment type" << id;
        }
    }
    foreach(FuzzyBase *fuzz,m_masterAgent->getBrain()->getFuzzies()) {
        if(fuzz->getType()==FuzzyBase::OUTPUT) {
            Output *origOut=(Output *)fuzz;
            agent->addOutputFuzz(origOut->getId(),origOut->getName(),origOut->getChannelName(),origOut->getEditorTranslationX(),origOut->getEditorTranslationY());
        } else {
            qDebug() <<  __PRETTY_FUNCTION__ << "missing fuzz type" << id;
        }
    }

    return agent;
}

bool AgentManager::loadConfig()
{
    QFile file(m_fileName);
    if(file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader reader;
        reader.setDevice(&file);
        while(reader.readNextStartElement()) {
            if(reader.name()=="Agentconfig") {
                while(reader.readNextStartElement()) {
                    if(reader.name()=="Agent") {
                        qDebug() << "Agemt Tag";
                        QXmlStreamAttributes attribs = reader.attributes();
                        setEditorTranslation(attribs.value("editorx").toString().toInt(),attribs.value("editory").toString().toInt());
                        setId(attribs.value("id").toString().toInt());
                        setName(attribs.value("name").toString());
                        while(reader.readNextStartElement()) {
                            if(reader.name()=="Body") {
                                qDebug() << "Body";
                                while(reader.readNextStartElement()) {
                                    qDebug()<< "Tag Name "<< reader.name();
                                    if(reader.name()=="Segment") {
                                        QXmlStreamAttributes attribs = reader.attributes();
                                        if( QString::compare( attribs.value("type").toString(),QString("sphere"),Qt::CaseInsensitive ) == 0 ) {
                                            qDebug() << "found SPhere" << attribs.value("id").toString();
                                            addSphereFromConfig(&reader,attribs.value("id").toString().toInt(),attribs.value("name").toString(),attribs.value("parent").toString().toInt());
                                            //reader.skipCurrentElement();
                                        }

                                    }else {
                                        reader.skipCurrentElement();
                                    }
                                }
                            } else if(reader.name()=="Brain") {
                                qDebug() << "parsing Brain";
                                while(reader.readNextStartElement()) {
                                    qDebug()<< "Tag Name "<< reader.name();
                                    if(reader.name()=="Output") {
                                        QXmlStreamAttributes attribs = reader.attributes();
                                        qDebug() << attribs.value("name");
                                        addOutputFuzz(attribs.value("id").toString().toInt(),attribs.value("name").toString(),attribs.value("channel").toString(),attribs.value("editorx").toString().toInt(),attribs.value("editory").toString().toInt());
                                        reader.skipCurrentElement();
                                    }else {
                                        reader.skipCurrentElement();
                                    }
                                }
                            } else {
                                reader.skipCurrentElement();
                            }
                        }
                    } else {
                        reader.skipCurrentElement();
                    }
                }
            } else {
                reader.skipCurrentElement();
            }
        }
        return true;
    }
    return false;
}

bool AgentManager::saveConfig()
{
    return false;
}

void AgentManager::setEditorTranslation(qint32 x, qint32 y)
{
    m_editX=x;
    m_editY=y;
}

/** \brief sets result of a fuzz of all agents
            each agent belonging to this manager is updated including its master agent
**/
void AgentManager::setFuzzyResult(quint32 id, qreal result)
{
    m_masterAgent->getBrain()->getFuzzy(id)->setResult(result);
    foreach(Agent *agent, m_group->getAgents()) {
        agent->getBrain()->getFuzzy(id)->setResult(result);
    }
}
