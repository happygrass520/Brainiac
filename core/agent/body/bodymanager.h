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

#ifndef BODYMANAGER_H
#define BODYMANAGER_H

#include "core/brainiacglobals.h"
#include "core/agent/body/segment.h"
#include "core/agent/body/segmentshape.h"
#include <QVector3D>
#include <QString>

class AgentManager;
class Agent;
class BodyEditor;

/**
 * @brief
 *
 * @class BodyManager bodymanager.h "core/agent/body/bodymanager.h"
 */
class BodyManager
{
public:
/**
 * @brief
 *
 * @fn BodyManager
 * @param manager
 */
    BodyManager(AgentManager *manager);

    /**
     * @brief applyBodyOnAgent
     * Apply the managed body to agent
     * @param agent the Agent this body should be applied
     */
    void applyBodyOnAgent(Agent *agent);

    /**
     * @brief returns a list of the Segment with id child ids
     * @param id the id of the Segment
     * @return QList<quint32> list of the child ids
     */
    QList<quint32> getChildIds(quint32 id) const;

    /**
     * @brief getSegment
     * returns the Segment
     *
     * If there is no Segment with given id an empty Segment with id 0 is returned
     * @param id the id of the segment
     * @return the Segment
     */
    Segment getSegment(quint32 id) const;

    /**
     * @brief getSegments returns all Segments
     * @return the Segment
     */
    QHash<quint32, SegmentShape *> getSegments() const { return m_segments; }

    /**
     * @brief getTraversedSegmentIds returns traversed child ids starting from the root segment
     * @return a list with traversed ids
     */
    QList<quint32> getTraversedSegmentIds() const;

    /**
     * @brief getSegmentChildIds
     * Returns the ids of all children of a Segment
     * @param id the id of the Segment
     * @return QList<quint32> an array of all ids
     */
    QList<quint32> getSegmentChildIds(quint32 id) const;

    /**
     * @brief returns zrue, if this segment has children
     *
     * @fn hasChildren
     * @return bool true, if segment has children
     */
    bool hasChildren(quint32 id) const;

    /**
     * @brief hasRootSegment
     * @return true, if this body has a root segment
     */
    bool hasRootSegment() const;

    /**
     * @brief
     *
     * @fn resetNewSegmentProperties
     */
    void resetNewSegmentProperties();

    /**
     * @brief sets new segment by another
     *
     * @fn setNewSegmentBySegment
     * @param other
     */
    void setNewSegmentBySegment(Segment &other);
    /**
     * @brief
     *
     * @fn setNewSegmentColor
     * @param color
     */
    void setNewSegmentColor(qreal color) { m_newSegment.setColor(color); }
    /**
     * @brief
     *
     * @fn setNewSegmentColorInherited
     * @param inherited
     */
    void setNewSegmentColorInherited(bool inherited) { m_newSegment.setColorInherited(inherited); }
    /**
     * @brief
     *
     * @fn setNewSegmentId
     * @param id
     */
    void setNewSegmentId(quint32 id) {m_newSegment.setId(id);}
    /**
     * @brief
     *
     * @fn setNewSegmentParentId
     * @param parent
     */
    void setNewSegmentParentId(quint32 parent) {m_newSegment.setParentId(parent);}
    /**
     * @brief
     *
     * @fn setNewSegmentTranslation
     * @param translation
     */
    void setNewSegmentTranslation(QVector3D &translation) {m_newSegment.setTranslation(translation);}
    /**
     * @brief
     *
     * @fn setNewSegmentRestTranslation
     * @param restTranslation
     */
    void setNewSegmentRestTranslation(QVector3D &restTranslation) {m_newSegment.setRestTranslation(restTranslation);}
    /**
     * @brief
     *
     * @fn setNewSegmentRotation
     * @param rotation
     */
    void setNewSegmentRotation(QVector3D &rotation) {m_newSegment.setRotation(rotation);}
    /**
     * @brief
     *
     * @fn setNewSegmentRestRotation
     * @param restRotation
     */
    void setNewSegmentRestRotation(QVector3D &restRotation) {m_newSegment.setRestRotation(restRotation);}
    /**
     * @brief
     *
     * @fn setNewSegmentSize
     * @param scale
     */
    void setNewSegmentSize(QVector3D &size) {m_newSegment.setSize(size);}
    /**
     * @brief
     *
     * @fn setNewSegmentType
     * @param type
     */
    void setNewSegmentType(BrainiacGlobals::SegmentType type) {m_newSegment.setType(type);}
    /**
     * @brief
     *
     * @fn setNewSegmentName
     * @param name
     */
    void setNewSegmentName(QString &name) {m_newSegment.setName(name);}
    /**
     * @brief
     *
     * @fn setNewSegmentRotationTranslationOrder
     * @param order
     */
    void setNewSegmentRotationTranslationOrder(QList<BrainiacGlobals::RotTrans> order );

    /**
     * @brief creates a new segment
     *
     * Before the new segment will be created, it will be checked if
     *   - if this is a root node and there is already another root node nothing is added and false will be returned.
     *   - if no id has been set, a unique id is created and registered
     *   - if the id is already set nothing is added and false will be returned
     *
     * @sa IdGenerator
     * @sa AgentManager
     *
     * @fn createNewSegment
     * @return bool
     */
    bool createNewSegment(bool processRootId=false);

    /**
     * @brief creates a new segment
     *
     * @return BodyEditor * a pointer to the BodyEditor
     */
    BodyEditor *getBodyEdtor() { return m_bodyEditor; }

    Segment getRootSegment() const;

    const QList<BrainiacGlobals::RotTrans>& getRootRotTransOrder() const { return m_rootSegmentRotTransOrder; }

    qreal getSegmentDiameter(quint32 id) const;

    qreal getSegmentLength(quint32 id) const;

    /**
     * @brief returns the level of this segment
     * the level of the root segment is 0
     *
     * @fn getSegmentLevel
     * @param id the id of the segment
     * @return int the level, -1 is returned in case of an error
     */
    int getSegmentLevel(quint32 id) const;

    QVector3D getSegmentRestRotation(quint32 id) const;

    QVector3D getSegmentRotation(quint32 id) const;

    QVector3D getSegmentSize(quint32 id) const;

    QVector3D getSegmentRestTranslation(quint32 id) const;

    QVector3D getSegmentTranslation(quint32 id) const;

    void setSegmentSize(quint32 id, qreal x, qreal y, qreal z);

    void setSegmentRestRotation(quint32 id, qreal x, qreal y, qreal z);

    void setSegmentRotation(quint32 id, qreal x, qreal y, qreal z);

    void setSegmentRestTranslation(quint32 id, qreal x, qreal y, qreal z);

    void setSegmentDiameter(quint32 id, qreal diameter);

    void setSegmentLength(quint32 id, qreal length);

    /**
     * @brief
     *
     * @fn setSegmentRotationTranslationOrder
     * @param order
     */
    void setSegmentRotationTranslationOrder(quint32 id, QList<BrainiacGlobals::RotTrans> order );

    void setSegmentTranslation(quint32 id, qreal x, qreal y, qreal z);

    void dDumpBody();

    /**
     * @brief
     * @fn ~BodyManager
     */
    ~BodyManager();
protected:

    /**
     * @brief Adds a SegmentShape to all Agents of this AgentManager
     *
     * @fn addSegmentToAgents
     * @param id
     */
    void addSegmentToAgents(quint32 id);

    /**
     * @brief addSegmentsToAgent
     * Adds all segments to an Agent
     * @param agent the Agent
     * @param startSegmentId the id of the segment to start the recursion
     */
    void addSegmentsToAgent(Agent *agent, quint32 startSegmentId);
    void getTraversedSegmentIdsRec(QList<quint32> *list, quint32 startId) const;

    AgentManager *m_agentManager; /**< The AgentManager this BodyManagers belongs to */
    Segment m_newSegment; /**< Temporary Segment */
    QHash<quint32, SegmentShape *> m_segments; /**< TODO */
    BodyEditor *m_bodyEditor; /**< the BodyEditor */
    QList<BrainiacGlobals::RotTrans> m_rootSegmentRotTransOrder;
};

#endif // BODYMANAGER_H
