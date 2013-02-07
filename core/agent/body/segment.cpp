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

#include "segment.h"
#include <osg/ShapeDrawable>


Segment::Segment()
{
        m_segmentRotTransOrder.append(BrainiacGlobals::RX);
        m_segmentRotTransOrder.append(BrainiacGlobals::RY);
        m_segmentRotTransOrder.append(BrainiacGlobals::RZ);

        m_segmentRotTransOrder.append(BrainiacGlobals::TX);
        m_segmentRotTransOrder.append(BrainiacGlobals::TY);
        m_segmentRotTransOrder.append(BrainiacGlobals::TZ);
}

Segment::Segment(const Segment &other):QObject()
{
    m_segmentRotation=other.getRotation();
    m_segmentTranslation=other.getTranslation();
    m_segmentRestRotation=other.getRestRotation();
    m_segmentRestTranslation=other.getRestTranslation();
    m_segmentScale=other.getScale();
    m_segmentId=other.getId();
    m_segmentParentId=other.getParentId();
    m_segmentType=other.getType();
    QObject::setObjectName(other.getName());
    m_segmentColorInherited=other.getColorInherited();
    m_segmentRotTransOrder=other.getRotationTranslationOrder();
}

void Segment::setRotationTranslationOrder(QList<BrainiacGlobals::RotTrans> list)
{
    m_segmentRotTransOrder.clear();
    m_segmentRotTransOrder=list;
    Q_ASSERT(m_segmentRotTransOrder.count()==6);
    emit updated();
}

Segment::~Segment()
{

}
