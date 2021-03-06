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


#ifndef BRAIN_H
#define BRAIN_H

#include <QObject>
#include "fuzzyand.h" // needed for Mode
#include "fuzzyor.h"
#include "fuzzyfuzz.h"
#include "timer.h"

class Agent;
class FuzzyBase;
class FuzzyDefuzz;
class Output;
class Input;
class Noise;


/** \brief  The agent´s brain

        Manages the agent´s brain

**/
class Brain : public QObject
{
    Q_OBJECT
public:
    explicit Brain(Agent *agent, Brain *brain);
    /** \brief adds an and fuzzy rule to this brain
                @param andFuzz the and rule to be added
    **/
    void addAndFuzz(FuzzyAnd *andFuzz);
    /** \brief adds an and fuzzy rule to this brain
    **/
    void addAndFuzz(quint32 id, FuzzyAnd::Mode mode);
    /** \brief adds a defuzz fuzzy rule to this brain
                @param defuzz the defuzz rule to be added
    **/
    void addDefuzz(FuzzyDefuzz *defuzz);
    /** \brief adds a defuzz rule to this brain
    **/
    void addDefuzz(quint32 id, qreal defuzzValue, bool isElse);
    void addFuzzFuzz(quint32 id, FuzzyFuzz::Mode mode, FuzzyFuzz::InterpolationMode intMode, qreal p1, qreal p2, qreal p3, qreal p4);
    void addFuzzFuzz(FuzzyFuzz *fuzzyFuzz);
    /** \brief adds an input to this brain
                @param input the input to be added
    **/
    void addInputFuzz(Input *out);
    /** \brief adds an input to this brain
                @param id the id of this fuzz node
                @param channel the name of the Channel this output is assigned to
                @param min the min value
                @param max the max value
    **/
    void addInputFuzz(quint32 id, const QString &channel, qreal min, qreal max);
    /** \brief adds an or fuzzy rule to this brain
                @param orFuzz the or rule to be added
    **/
    void addOrFuzz(FuzzyOr *orFuzz);
    /** \brief adds an or fuzzy rule to this brain
                @param orFuzz the or rule to be added
    **/
    void addOrFuzz(quint32 id, FuzzyOr::Mode mode);
    /** \brief adds an output to this brain
    **/
    void addOutputFuzz(Output *out);
    /** \brief adds an output to this brain
    **/
    void addOutputFuzz(quint32 id, const QString &channel, qreal min, qreal max);
    /** \brief adds a noise fuzz to this brain
                @param noise the noise to be added
    **/
    void addNoiseFuzz(Noise *noise);
    /** \brief adds a noise fuzz to this brain
                @param id the the id of this fuzz node
                @param the rate of this noise node
    **/
    void addNoiseFuzz(quint32 id, qreal rate);
    void addTimerFuzz(Timer *timer);
    void addTimerFuzz(quint32 id, qreal rate, Timer::TimerMode mode);

    /** \brief connect two fuzz nodes
                Connect a child to a parent. After the fuzzies are connected, a calculation of the child is issued to fetch the value of the parent(s)
                @param childId the fuzz to receive the output/result
                @param parentId the fuzz to send it´s result
                @param inverted if the parent´s result must be inverted
                @sa FuzzyBase
    **/
    void connectFuzzies(quint32 childId, quint32 parentId, bool inverted);

    /** \brief deletes a fuzz from this brain

                    all connecttions from/to this fuzz´ brain are also deleted

            \param  fuzzId the id of the fuzz to be deleted
    **/
    void deleteFuzz(quint32 fuzzId);

    /** \brief disconnect two fuzz nodes
                disconnects the fuzzies and removes their child/parent entries
                @param childId the fuzz to receive the output/result
                @param parentId the fuzz to send it´s result
                @sa FuzzyBase
    **/
    void disconnectFuzzies(quint32 childId, quint32 parentId);

    /** \brief returns the agent this brain belongs to

                    @returns the agent
    **/
    Agent* getAgent() const;
    /** \brief returns the pointer to the fuzzie´s is

                    @returns the pointer to the fuzz node

    **/
    FuzzyBase* getFuzzy(quint32 id) const;

    /** \brief returns the pointer to the fuzzie´s is
                    The fuzzy is returned that first matches the name
                    @param name the name of the fuzz
                    @returns the pointer to the fuzz node

    **/
    FuzzyBase* getFuzzyByName(const QString& name);

    QList <FuzzyBase *> getFuzzies() const;
    ~Brain();

protected:
    QList <FuzzyBase *> m_fuzzies;
signals:

public slots:

};

#endif // BRAIN_H
