#include "channellistpanel.h"
#include "ui_channellistpanel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QMenu>

ChannelListPanel::ChannelListPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelListPanel),
    m_model(new ChannelModel)
{
    ui->setupUi(this);
    ui->m_channelView->setModel(m_model);
    ui->m_channelView->setAlternatingRowColors(true);
    ui->m_channelView->setHeaderHidden(true);

    ui->m_channelView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->m_channelView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showCustomMenu(QPoint)));

    m_edit= new QAction(tr("Edit Channel"),this);
    m_lock= new QAction(tr("Lock Channel"),this);
    m_join= new QAction(tr("Switch to channel"),this);
    m_addChannel= new QAction(tr("Add channel"),this);
    m_addSubchannel= new QAction(tr("Add subchannel"),this);
    m_deleteChannel= new QAction(tr("Delete Channel"),this);
    m_setDefault = new QAction(tr("Set Default"),this);
    m_admin = new QAction(tr("Log as admin"),this);
    m_kick = new QAction(tr("Kick User"),this);
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
                //qDebug() << "[Received channel] " << doc.toJson();
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
void ChannelListPanel::showCustomMenu(QPoint pos)
{
    QMenu menu(this);

    menu.addAction(m_join);
    menu.addSeparator();
    menu.addAction(m_setDefault);
    menu.addAction(m_edit);
    menu.addAction(m_lock);
    menu.addSeparator();
    menu.addAction(m_addChannel);
    menu.addAction(m_addSubchannel);
    menu.addAction(m_deleteChannel);
    menu.addAction(m_admin);

    QModelIndex index = ui->m_channelView->indexAt(pos);

    if((!index.isValid())||(ChannelListPanel::VIEWER == m_currentGRoup))
    {
        m_setDefault->setEnabled(false);
        m_edit->setEnabled(false);
        m_lock->setEnabled(false);
        m_addChannel->setEnabled(false);
        m_deleteChannel->setEnabled(false);
        m_addSubchannel->setEnabled(false);
    }



    menu.exec(ui->m_channelView->mapToGlobal(pos));
}

void ChannelListPanel::kickUser()
{

}

void ChannelListPanel::banUser()
{

}

void ChannelListPanel::addChannel()
{

}

void ChannelListPanel::deleteChannel()
{

}

void ChannelListPanel::joinChannel()
{

}

ChannelListPanel::GROUP ChannelListPanel::currentGRoup() const
{
    return m_currentGRoup;
}

void ChannelListPanel::setCurrentGRoup(const GROUP &currentGRoup)
{
    m_currentGRoup = currentGRoup;
}
