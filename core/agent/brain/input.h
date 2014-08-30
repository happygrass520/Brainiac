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


#ifndef INPUT_H
#define INPUT_H

#include "core/agent/brain/fuzzybase.h"

class Channel;

/** \brief  Input fuzz node

        this class describes and manages an input node in the agent´s brain.
        if this node is associated with an input channel, it reads the calculated results out

**/

class Input : public FuzzyBase
{
    Q_OBJECT
public:
    Input(quint32 id, Brain *brain, QString name, QString channel, qreal min, qreal max);
    /** \brief  @return the channel name
    **/
    QString getChannelName();

    void calculate();

    /** @brief returns if this is a sound input
      @return true if this is a sound input
    **/
    bool isSoundInput() const { return m_isSoundInput; }

    /** \brief  set the this input as a sound input
      @param isSound true, if this input is a sound Input
    **/
    void setIsSoundInput(bool isSound) { m_isSoundInput=isSound; }

    /** \brief  set the channel of this input
      @param channel the name of the channel assigned to this Input
    **/
    void setChannelName(QString channel);

protected slots:
    void channelDelete();

protected:
    QString m_channelName; //!< name of this input node
    Channel *m_channel; //!< a pointer to the channel, null if no channel is assigned
    bool m_isSoundInput; //!< true, if a sound channel is assigned
};


#endif // INPUT_H
