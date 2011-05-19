#ifndef SEGMENT_H
#define SEGMENT_H

#include "core/brainiacglobals.h"
#include <QList>
#include <QString>

class Body;
class Channel;
class QVector3D;

class BrainiacColor;

class Segment
{
public:
    enum SegmentType {SPHERE=BrainiacGlobals::SPHERE, CUBE=BrainiacGlobals::CUBE};
    Segment(SegmentType type, quint32 id, Body *body, QString name, QVector3D *restRot, QVector3D *restTrans, Segment *parent=0);
    virtual ~Segment();
    void addChild(Segment *segment);
    SegmentType getType() { return m_type; }
    quint32 getId() { return m_id; }
    virtual void reset();
    void setName(const QString & name);

protected:
    virtual void createSegmentChannels();
    quint32 m_id;
    Segment *m_parent;
    Body *m_body;
    qreal m_restColor; //!< color of rest pose
    SegmentType m_type;
    QList<Segment *> m_children;
    QString m_name;
    QVector3D *m_restRotation; //!< Rotation of rest pose
    QVector3D *m_restTranslation; //!< Translation of rest pose
    Channel *m_tx; //!< x translation (input and output)
    Channel *m_ty; //!< y translation (input and output)
    Channel *m_tz; //!< z translation (input and output)
    Channel *m_rx; //!< x rotation (input and output)
    Channel *m_ry; //!< y rotation (input and output)
    Channel *m_rz; //!< z rotation (input and output)
    Channel *m_scale; //!<  scale factor of this segment (input and output)
};

#endif // SEGMENT_H