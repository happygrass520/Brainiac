#ifndef MOTIONTREETRANSITION_H
#define MOTIONTREETRANSITION_H

class MotionTree;
class MotionTreeEditorItem;

class MotionTreeTransition
{
public:
    MotionTreeTransition(MotionTree *tree);
    virtual ~MotionTreeTransition();

protected:
    MotionTree *m_motionTree;
    MotionTreeEditorItem *m_editorItem;
};

#endif // MOTIONTREETRANSITION_H