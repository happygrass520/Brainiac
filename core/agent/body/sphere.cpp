#include "sphere.h"
#include "core/agent/agent.h"
#include "core/agent/body/body.h"
#include "core/agent/channel.h"

Sphere::Sphere(quint32 id, Body *body, QString name, QVector3D *restRot, QVector3D *restTrans, qreal radius, Segment *parent)
    :Segment(Segment::SPHERE,id,body,name,restRot,restTrans,parent)
{
    m_restRadius=radius;
    createSegmentChannels();
}

Sphere::~Sphere()
{
    m_body->getAgent()->deleteChannel(m_radius);
}

void Sphere::createSegmentChannels()
{
    m_radius=new Channel();
    m_radius->init(m_restRadius);
    QString name=m_name % ":radius";
    m_body->getAgent()->addInputChannel(m_radius,name);
    m_body->getAgent()->addOutputChannel(m_radius,name);

}

void Sphere::reset()
{
    Segment::reset();
    m_radius->init(m_restRadius);
}
