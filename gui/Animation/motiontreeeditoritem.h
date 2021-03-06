#ifndef MOTIONTREEEDITORITEM_H
#define MOTIONTREEEDITORITEM_H

#include "gui/editoritem.h"
#include "gui/editorlabel.h"
#include "core/brainiacglobals.h"

class MotionTreeEditorItem : public EditorItem
{
public:
    MotionTreeEditorItem(BrainiacGlobals::ItemType type, void *object);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual ~MotionTreeEditorItem();
};

#endif // MOTIONTREEEDITORITEM_H
