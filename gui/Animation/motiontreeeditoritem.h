#ifndef MOTIONTREEEDITORITEM_H
#define MOTIONTREEEDITORITEM_H

#include "gui/editoritem.h"
#include "gui/editorlabel.h"
#include "core/brainiacglobals.h"

class MotionTreeEditorItem : public EditorItem
{
public:
    MotionTreeEditorItem(BrainiacGlobals::ItemType type, void *object,quint32 id);
};

#endif // MOTIONTREEEDITORITEM_H
