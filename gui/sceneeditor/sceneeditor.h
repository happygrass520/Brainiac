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


#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include <QGraphicsScene>
#include "gui/editorbase.h"


class QGraphicsSceneDragDropEvent;

/** \brief  The graphical editor for this scene´s elements

        This editor allows easy selecting of the main contents of this scene
        The selected group also is the agent instance to be edited in braineditor and bodyeditor
        @sa BrainEditor

**/
class SceneEditor : public EditorBase
{
    Q_OBJECT
public:
    SceneEditor(Scene *scene);
    void refresh();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

};

#endif // SCENEEDITOR_H
