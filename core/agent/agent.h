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


#ifndef AGENT_H
#define AGENT_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QVector3D>
#include "brain/fuzzyand.h" // needed for Mode
#include "brain/fuzzyor.h"
#include "brain/fuzzyfuzz.h"
#include "brain/timer.h"
#include "channel.h"

class Body;
class Brain;
class Scene;
class Segment;
class AgentManager;
class Locator;

/** \brief  Main agent class

        This class holds all properties of an agent.
        It holds the agent´s brain, actions and body.
        The class also makes the agent interact with other scene content
        like other agents, terrain, colors etc.
        @sa Body
        @sa Brain
        @sa Scene
        @todo rename of "rest" members/function
        @todo rename channel functions to "getChannel*" etc

**/
class Agent : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief describes the processing
     *
     * @enum AdvanceMode
     */
    enum AdvanceMode {
        NONE=0, //!< Nothing is calculated
        BRAIN=1, //!< Brain calculation
        ALL=BRAIN //!< everything is calculated
    };

    /** \brief Constructs new Agent instance

                    This constructor is used to create a new agent instance belonging to the scene
                    @param scene a pointer to the scene
                    @param id the id of the agent

    **/
    Agent(AgentManager *manager, quint32 id);

    /** \brief Constructs new Agent instance as a copy of an existing agent

                    This constructor is used to create a new agent which is an exact copy of given agent except the id.
                    @param otherAgent a pointer to the original agent
                    @param id the id of the agent
    **/
    explicit Agent(Agent *otherAgent, quint32 id=0);

    /** \brief adds an input channel

                    adds an input channel to the agent.
                    If channel already exists nothing is added
                    \return true, if adding was successful, false if not
    **/
    bool addInputChannel(Channel* channel, const QString &name);

    /** \brief adds an output channel

                    adds an output channel to the agent.
                    If channel already exists nothing is added
                    \param  channel pointer to output channel
                    @param name name of the channel
                    \return true, if adding was successful, false if not

    **/
    bool addOutputChannel(Channel* channel, const QString &name);

    /** \brief get an output channel, if the channel does not exist, a new channel is created and added as an output channel

                    @param name name of the channel
                    \return pointer to the channel

    **/
    Channel* getOrCreateOutputChannel(const QString &name);

    /** \brief connects to fuzz rules

                    @param child the id of the child of the connection (the receiver)
                    @param parentId the id of the parent of the connection (the sender)
                    @param inverted true, if this connection should be inverted (the result will be inverted)

    **/
    void addConnection(quint32 child, quint32 parentId, bool inverted);
    // Fuzz Stuff
    void addAndFuzz(quint32 id, FuzzyAnd::Mode mode);
    void addDefuzz(quint32 id, qreal defuzzValue, bool isElse);
    void addFuzzFuzz(quint32 id, FuzzyFuzz::Mode mode, FuzzyFuzz::InterpolationMode intMode, qreal p1, qreal p2, qreal p3, qreal p4);
    void addInputFuzz(quint32 id, const QString &channel, qreal min, qreal max);
    void addOrFuzz(quint32 id, FuzzyOr::Mode mode);
    void addOutputFuzz(quint32 id, const QString &channel, qreal min, qreal max);
    void addNoiseFuzz(quint32 id, qreal rate);
    void addTimerFuzz(quint32 id, qreal rate, Timer::TimerMode mode);
    /** \brief advances this agent

                    calling this function makes the agent go one step further in time
                    Typically this function is called from the Simulation class

                    It queries infos of all agents in the scene and the scene´s environment
                    It triggers its brain to process

                    The changes have to be written down after all agents have calculated their
                    new values via advanceCommit()

                    @sa Simulation
                    @sa Brain
                    @sa Agent::advanceCommit()

    **/
    void advance(AdvanceMode mode=Agent::ALL);
    /** \brief commits all changes calculated by advance

                    all calculated channels etc are written down and are "baked", the new values becoming the actual values
                    @sa Agent::advance()

    **/
    void advanceCommit();

    /**
     * @brief creates or updates the list containing the pointers of the traversed segment channels according to bvh format
     *
     * this list is used to provide quick access to the output channels of all segments
     *
     * @fn createBVHChannelList
     */
    void createBVHChannelList();

    /** \brief deletes channel

                    the channel is deleted both from inputs and outputs.
                    also its destructor is invoked
                    @param channel the channel to be deleted

    **/
    void deleteChannel(Channel* channel);
    /** \brief deletes a connection between two fuzzies
        @param childId the id of the child/the receiver
        @param parentId the id of the parent/the sender
    **/
    void deleteConnection(quint32 parentId, quint32 childId);

    /** \brief deletes a fuzz from this agent´s brain

                    all connecttions from/to this fuzz´ brain are also deleted

            \param  fuzzId the id of the fuzz to be deleted
    **/
    void deleteFuzz(quint32 fuzzId);

    /** \brief returns the AgentManager this agent belongs to
    **/
    AgentManager *getAgentManager() const { return m_agentManager; }

    /** \brief returns the body
    **/
    Body *getBody() const;

    /**
     * @brief returns a string containing the channel values of all agent segments in proper order for bvh file
     *
     * @sa createBVHChannelList()
     * @fn getBvhMotionData
     * @return QString a string with blank seperated channel values
     */
    QString &getBvhMotionData() const;

    /** \brief returns the brain
    **/
    Brain *getBrain() const;

    /** \brief returns the bodie´s color
            by default this value is inherited recursivly to all segments of the body
            @sa Segment::setColorInherited(bool inherited)
    **/
    Channel *getColor() const;

    /** \brief returns input channel
        @param name the name of the input channel
        @param options behavior, if channel does not exit
        @returns 0 if channel does not exist
    **/
    Channel *getInputChannel(const QString &name, Channel::ChannelNotExistOptions options=Channel::NONE);

    /**
     * @brief returns the name of the input channel
     *
     * @fn getInputChannelName
     * @param channel pointer of the channel
     * @return QString name of the channel, empty String if channel was not found
     */
    QString getInputChannelName(const Channel *channel) const;

    /** \brief returns output channel
        @param name the name of the output channel
        @returns 0 if channel does not exist
    **/
    Channel *getOutputChannel(const QString &name) const;
    Channel *getOutputChannel(const QString &name, Channel::ChannelNotExistOptions options=Channel::NONE);

    /**
     * @brief returns the name of the output channel
     *
     * @fn getOutputChannelName
     * @param channel pointer of the channel
     * @return QString name of the channel, empty String if channel was not found
     */
    QString getOutputChannelName(const Channel *channel) const;

    /**
     * @brief returns the id of this agent
     *
     * @return quint32 the id
     */
    quint32 getId() const;

    /** \brief returns direction(angle) sound is received from horizontally

      polar coordinate about the Y axis of sound source. The unit is degree
      @param otherAgent the agent to which the relative angle is calculated
    **/
    qreal getOtherAgentRelativeAngle(const Agent *otherAgent) const;

    /**
     * @brief returns the orientation relative to its own
     * relative orientation of sound source (which way target is facing relative to self)
     * @param otherAgent the other agent
     * @return qreal the relative angle
     */
    qreal getOtherAgentRelativeOrientation(const Agent *otherAgent) const;

    /** \brief returns the sound reception of given agent

      @param otherAgent the agent to which the reception is calculated
    **/
    qreal getOtherAgentSoundReception(const Agent *otherAgent) const;

    /** \brief returns the position given agent relative to current position

      @param otherAgent the agent to which the position is calculated
    **/
    QVector3D getOtherAgentRelativePosition(const Agent *otherAgent) const;

    /** \brief returns the position of the agent in world space
    **/
    const QVector3D& getPosition() const;
    /** \brief returns the rotation of the agent in world space
    **/
    const QVector3D& getRotation() const;
    /** \brief true if sound emmisions should be rendered
    **/
    bool getRenderSoundEmission() const;
    /** \brief returns the scene
        @sa Scene
    **/
    Scene *getScene() const;
    /** \brief @returns true if channel exists
    **/
    bool inputChannelExists(const QString &name) const;
    /** \brief @returns true if channel exists
    **/
    bool outputChannelExists(const QString &name) const;

    void renderSoundEmission(bool render);
    void reset();

    /**
     * @brief sets the name of the file to write the bvh motion data
     *
     * @fn setBVHFileName
     * @param fileName the file name
     */
    void setBVHFileName( const QString& fileName ) {
        m_bvh_fileName=fileName;
    }

    /**
     * @brief setObjectName sets the name of the agent
     *
     * @param name the name of the agent
     */
    void setObjectName(const QString &name);
    void setRotation(qreal x, qreal y, qreal z);
    void setTranslation(qreal x, qreal y, qreal z);
    void setRestRotation(qreal x, qreal y, qreal z);
    void setRestTranslation(qreal x, qreal y, qreal z);

    void writeBVHMotionData();


    /**
     * @brief dumps list of all channels
     *
     * @fn dDumpChannels
     */
    void dDumpChannels();
    virtual ~Agent();

private:
    AgentManager *m_agentManager;//!< the AgentManager this Agent belongs to
    Body *m_body; //!< the agent´s body
    Brain *m_brain; //!< the agent´s brain
    quint32 m_id; //!< the agent´s id. Unique for its group
    QHash<QString, Channel *> m_inputs; //!< List of all input channels of this agent
    QHash<QString, Channel *> m_outputs; //!< List of all output channels of this agent
    void createChannels();
    // Channels
    Channel *m_color; //!< color of agent (input and output)
    Channel *m_rx; //!< x rotation rate of agent (input and output)
    Channel *m_ry; //!< y rotation rate of agent (input and output)
    Channel *m_rz; //!< z rotation rate of agent (input and output)
    Channel *m_tx; //!< x translation rate of agent (input and output)
    Channel *m_ty; //!< y translation rate of agent (input and output)
    Channel *m_tz; //!< z translation rate of agent (input and output)
    Channel *m_oSoundF; //!< Emmited frequency of sound (output)
    Channel *m_oSoundA; //!< Emmited amplitude of sound (output)
    Channel *m_iSoundX; //!< polar coordinate about the Y axis of sound source (input)
    Channel *m_iSoundOX; //!< relative angle of receiving sound source (input)
    Channel *m_iSoundD; //!< distance of received sound;
    Channel *m_iSoundF; //!< frequency of received sound;

    QList<Channel *> m_bvhChannelList; //!< the list of the agent channels according to bvh order
    QString m_bvh_fileName; //!< the name of the file to write bvh motion data
    mutable QString m_bvhMotionData; //!< contains actual bvh motion data


    Locator *m_locator; //!< the locator that holds the initial position and rotation
    Scene *m_scene; //!< the scene

    QVector3D m_position; //!< Agent current position in world space
    QVector3D m_rotation; //!< Agent current rotation in world space
    QVector3D m_newPosition; //!< new agent position in world space
    QVector3D m_newRotation; //!< new agent rotation in world space

    bool m_renderSoundEmission; //!< toogle render sound emissions

signals:

public slots:
private:
    Q_DISABLE_COPY(Agent)
};

#endif // AGENT_H
