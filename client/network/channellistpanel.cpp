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
    m_model(new ChannelModel),
    m_currentGRoup(ADMIN)
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

    connect(m_kick,SIGNAL(triggered(bool)),this,SLOT(kickUser()));
    connect(m_edit,SIGNAL(triggered(bool)),this,SLOT(editChannel()));
    connect(m_addChannel,SIGNAL(triggered(bool)),this,SLOT(addChannelAsSibbling()));
    connect(m_addSubchannel,SIGNAL(triggered(bool)),this,SLOT(addChannel()));
    connect(m_deleteChannel,SIGNAL(triggered(bool)),this,SLOT(deleteChannel()));
    connect(m_lock,SIGNAL(triggered(bool)),this,SLOT(lockChannel()));
    connect(m_join,SIGNAL(triggered(bool)),this,SLOT(joinChannel()));
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
    enum ClickState {Out,OnChannel,OnUser};

    ClickState state = Out;
    menu.addAction(m_join);
    menu.addAction(m_kick);
    menu.addSeparator();
    menu.addAction(m_setDefault);
    menu.addAction(m_edit);
    menu.addAction(m_lock);
    menu.addSeparator();
    menu.addAction(m_addChannel);
    menu.addAction(m_addSubchannel);
    menu.addAction(m_deleteChannel);
    menu.addAction(m_admin);

    m_index = ui->m_channelView->indexAt(pos);


    if(!m_index.isValid())
    {
        state = Out;
    }
    else
    {
        TreeItem* data = indexToPointer<TreeItem*>(m_index);
        if(data->isLeaf())
        {
            state = OnUser;
        }
        else
        {
            state = OnChannel;
        }
    }
    if(state== Out)
    {
        m_addChannel->setEnabled(true);
        m_edit->setEnabled(false);
        m_lock->setEnabled(false);
        m_deleteChannel->setEnabled(false);
        m_addSubchannel->setEnabled(false);
        m_kick->setEnabled(false);
        m_setDefault->setEnabled(false);
        m_join->setEnabled(false);

    }
    else if(state == OnChannel)
    {
        m_kick->setEnabled(false);
        m_setDefault->setEnabled(true);
        m_edit->setEnabled(true);
        m_lock->setEnabled(true);
        m_addChannel->setEnabled(true);
        m_deleteChannel->setEnabled(true);
        m_addSubchannel->setEnabled(true);
        m_join->setEnabled(true);



    }
    else if(state == OnUser)
    {
        m_kick->setEnabled(true);
        m_edit->setEnabled(false);
        m_lock->setEnabled(false);
        m_deleteChannel->setEnabled(false);
        m_addSubchannel->setEnabled(false);
        m_setDefault->setEnabled(false);
        m_join->setEnabled(false);
        m_addChannel->setEnabled(false);
    }


    if(ChannelListPanel::VIEWER == m_currentGRoup)
    {
        m_kick->setEnabled(false);
        m_edit->setEnabled(false);
        m_lock->setEnabled(false);
        m_deleteChannel->setEnabled(false);
        m_addSubchannel->setEnabled(false);

    }
    menu.exec(ui->m_channelView->mapToGlobal(pos));
}
bool ChannelListPanel::isAdmin()
{
    /// @TODO manage the admin account.
    if(ChannelListPanel::ADMIN == m_currentGRoup)
    {
        return true;
    }
    return false;
}
#include "tcpclient.h"

void ChannelListPanel::kickUser()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            TcpClient* item = static_cast<TcpClient*>(m_index.internalPointer());
            QString id = item->getId();
            QString idPlayer = item->getPlayerId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::Kicked);
                QStringList idList;
                idList << id;
                msg.setRecipientList(idList,NetworkMessage::OneOrMany);
                msg.string8(id);
                msg.string8(idPlayer);
                msg.sendAll();
            }
        }

    }
}
void ChannelListPanel::lockChannel()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            Channel* item = static_cast<Channel*>(m_index.internalPointer());
            QString id = item->getId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::LockChannel);
                msg.string8(id);
                msg.sendAll();
            }
        }
    }
}
template <typename T>
T ChannelListPanel::indexToPointer(QModelIndex index)
{
    T item = static_cast<T>(index.internalPointer());
    return item;
}

void ChannelListPanel::banUser()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            TcpClient* item = indexToPointer<TcpClient*>(m_index);///static_cast<TcpClient*>(m_index.internalPointer());
            QString id = item->getId();
            QString idPlayer = item->getPlayerId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::BanUser);
                msg.string8(id);
                msg.string8(idPlayer);
                msg.sendAll();
            }
        }
    }
}

void ChannelListPanel::addChannel()
{
    if(isAdmin())
    {
        Channel* newChannel = new Channel(tr("New Channel"));

        Channel* parent = indexToPointer<Channel*>(m_index);

        QModelIndex justAdded = m_model->addChannelToIndex(newChannel,m_index);
        ui->m_channelView->edit(justAdded);

        if(nullptr!=parent)
        {
            QString parentId = parent->getId();
            if(!parentId.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::AddChannel);
                msg.string8(parentId);
                newChannel->fill(msg);
                msg.sendAll();
            }
        }
    }
}
void ChannelListPanel::addChannelAsSibbling()
{
    if(isAdmin())
    {
        Channel* newChannel = new Channel(tr("New Channel"));

        auto parentIndex = m_index.parent();

        Channel* parent = nullptr;
        QString parentId("");

        if(parentIndex.isValid())
        {
            parent = indexToPointer<Channel*>(parentIndex);
            parentId = parent->getId();

        }

        QModelIndex justAdded = m_model->addChannelToIndex(newChannel,parentIndex);
        ui->m_channelView->edit(justAdded);

        NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::AddChannel);
        msg.string8(parentId);
        newChannel->fill(msg);
        msg.sendAll();

    }
}

void ChannelListPanel::editChannel()
{
    // Todo display dialog to edit properties of QML
    if(isAdmin())
    {
        ui->m_channelView->edit(m_index);
    }
}
void ChannelListPanel::deleteChannel()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            Channel* item = static_cast<Channel*>(m_index.internalPointer());
            QString id = item->getId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::DeleteChannel);
                msg.string8(id);
                msg.sendAll();
            }
        }
    }
}

void ChannelListPanel::joinChannel()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            Channel* item = static_cast<Channel*>(m_index.internalPointer());
            QString id = item->getId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory,NetMsg::JoinChannel);
                msg.string8(id);
                msg.sendAll();
            }
        }
    }
}

ChannelListPanel::GROUP ChannelListPanel::currentGRoup() const
{
    return m_currentGRoup;
}

void ChannelListPanel::setCurrentGRoup(const GROUP &currentGRoup)
{
    m_currentGRoup = currentGRoup;
}
