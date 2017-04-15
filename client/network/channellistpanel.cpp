#include "channellistpanel.h"
#include "ui_channellistpanel.h"

ChannelListPanel::ChannelListPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelListPanel)
{
    ui->setupUi(this);
}

ChannelListPanel::~ChannelListPanel()
{
    delete ui;
}
