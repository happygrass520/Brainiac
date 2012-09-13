#include "andeditor.h"
#include "ui_andeditor.h"

AndEditor::AndEditor(Scene *scene, QWidget *parent) :
    QWidget(parent),
    ItemEditorWidgetsBase(scene),
    ui(new Ui::AndEditor)
{
    ui->setupUi(this);
}

AndEditor::~AndEditor()
{
    delete ui;
}
