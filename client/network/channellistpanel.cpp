#include "channellistpanel.h"
#include "ui_channellistpanel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

ChannelListPanel::ChannelListPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelListPanel),
    m_model(new ChannelModel)
{
    ui->setupUi(this);
    ui->m_channelView->setModel(m_model);


}

ChannelListPanel::~ChannelListPanel()
{
    delete ui;
}
void ChannelListPanel::processMessage(NetworkMessageReader* msg)
{
    switch (msg->action())
    {
        case NetMsg::Goodbye:

        break;
        case NetMsg::Kicked:

        break;
        case NetMsg::MoveChannel:

        break;
        case NetMsg::SetChannelList:
        {
            QByteArray data = msg->byteArray32();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            if(!doc.isEmpty())
            {
                QJsonObject obj = doc.object();

                m_model->readDataJson(obj);
            }

        }
            break;
        default:
            break;
    }
}

void ChannelListPanel::sendOffModel()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::SetChannelList);
    QJsonDocument doc;
    QJsonObject obj;
    m_model->writeDataJson(obj);
    doc.setObject(obj);

    msg.byteArray32(doc.toJson());
    msg.sendAll();
}
