#include "segment.h"
#include "core/brainiaccolor.h"
#include "core/agent/body/body.h"
#include "core/agent/agent.h"
#include "core/agent/channel.h"
#include <QVector3D>


Segment::Segment(SegmentType type, quint32 id, Body *body, QString name, QVector3D *restRot, QVector3D *restTrans, Segment *parent)
{
    m_type=type;
    m_id=id;
    m_parent=parent;
    //m_scale=1.0;
    m_body=body;
    m_name=name;
    m_restRotation=restRot;
    m_restTranslation=restTrans;
    //m_color=new BrainiacColor();
    //m_color=0.5; //!< \todo color management
    createSegmentChannels();
    if(parent) {
        parent->addChild(this);
    }
}
Segment::~Segment()
{
    delete m_restRotation;
    delete m_restTranslation;
    m_body->getAgent()->deleteChannel(m_tx);
    m_body->getAgent()->deleteChannel(m_ty);
    m_body->getAgent()->deleteChannel(m_tz);
    m_body->getAgent()->deleteChannel(m_rx);
    m_body->getAgent()->deleteChannel(m_ry);
    m_body->getAgent()->deleteChannel(m_rz);
}

void Segment::addChild(Segment *segment)
{
    m_children.append(segment);
}

void Segment::createSegmentChannels()
{
    m_tx=new Channel();
    m_tx->init(m_restTranslation->x());
    QString name=m_name % ":tx";
    m_body->getAgent()->addInputChannel(m_tx,name);

    m_ty=new Channel();
    m_ty->init(m_restTranslation->y());
    name=m_name % ":ty";
    m_body->getAgent()->addInputChannel(m_ty,name);

    m_tz=new Channel();
    m_tz->init(m_restTranslation->z());
    name=m_name % ":tz";
    m_body->getAgent()->addInputChannel(m_tz,name);

    m_rx=new Channel();
    m_rx->init(m_restRotation->x());
    name=m_name % ":rx";
    m_body->getAgent()->addInputChannel(m_rx,name);

    m_ry=new Channel();
    m_ry->init(m_restRotation->y());
    name=m_name % ":ry";
    m_body->getAgent()->addInputChannel(m_ry,name);

    m_rz=new Channel();
    m_rz->init(m_restRotation->z());
    name=m_name % ":rz";
    m_body->getAgent()->addInputChannel(m_rz,name);
}

/** \brief reset this segment

                resets this segement´s rotation and translation etc. to its rest pose

**/
void Segment::reset()
{
    m_tx->init(m_restTranslation->x());
    m_ty->init(m_restTranslation->y());
    m_tz->init(m_restTranslation->z());

    m_rx->init(m_restRotation->x());
    m_ry->init(m_restRotation->y());
    m_rz->init(m_restRotation->z());
}

void Segment::setName(const QString &name) {
    m_name=name;
}