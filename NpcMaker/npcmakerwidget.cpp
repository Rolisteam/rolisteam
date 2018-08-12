#include "npcmakerwidget.h"
#include "ui_npcmakerwidget.h"

NpcMakerWidget::NpcMakerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NpcMakerWidget)
{
    ui->setupUi(this);
}

NpcMakerWidget::~NpcMakerWidget()
{
    delete ui;
}
