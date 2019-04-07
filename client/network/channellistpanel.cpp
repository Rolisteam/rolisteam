#include "channellistpanel.h"
#include "ui_channellistpanel.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>

#include "preferences/preferencesmanager.h"
#include "tcpclient.h"

ChannelListPanel::ChannelListPanel(QWidget* parent)
    : QWidget(parent), ui(new Ui::ChannelListPanel), m_model(new ChannelModel), m_currentGroups(VIEWER)
{
    ui->setupUi(this);
    ui->m_channelView->setModel(m_model.get());
    ui->m_channelView->setAlternatingRowColors(true);
    ui->m_channelView->setHeaderHidden(true);
    ui->m_channelView->setAcceptDrops(true);
    ui->m_channelView->setDragEnabled(true);
    ui->m_channelView->setDropIndicatorShown(true);
    ui->m_channelView->setDefaultDropAction(Qt::MoveAction);
    ui->m_channelView->setDragDropMode(QAbstractItemView::InternalMove);

    ui->m_channelView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->m_channelView, &QTreeView::customContextMenuRequested, this, &ChannelListPanel::showCustomMenu);
    connect(m_model.get(), &ChannelModel::localPlayerGMChanged, this, &ChannelListPanel::CurrentChannelGmIdChanged);

    m_edit= new QAction(tr("Edit Channel"), this);
    m_lock= new QAction(tr("Lock/Unlock Channel"), this);
    m_join= new QAction(tr("Switch to channel"), this);
    m_channelPassword= new QAction(tr("Set/Unset Channel Password"), this);
    m_addChannel= new QAction(tr("Add channel"), this);
    m_addSubchannel= new QAction(tr("Add subchannel"), this);
    m_deleteChannel= new QAction(tr("Delete Channel"), this);
    m_setDefault= new QAction(tr("Set Default"), this);
    m_admin= new QAction(tr("Log as admin"), this);
    m_kick= new QAction(tr("Kick User"), this);
    m_ban= new QAction(tr("Ban User"), this);
    m_resetChannel= new QAction(tr("Reset Data Channel"), this);
    m_moveUserToCurrentChannel= new QAction(tr("Move User"), this);

    connect(m_kick, &QAction::triggered, this, &ChannelListPanel::kickUser);
    connect(m_ban, &QAction::triggered, this, &ChannelListPanel::banUser);
    connect(m_edit, &QAction::triggered, this, &ChannelListPanel::editChannel);
    connect(m_addChannel, &QAction::triggered, this, &ChannelListPanel::addChannelAsSibbling);
    connect(m_addSubchannel, &QAction::triggered, this, &ChannelListPanel::addChannel);
    connect(m_deleteChannel, &QAction::triggered, this, &ChannelListPanel::deleteChannel);
    connect(m_lock, &QAction::triggered, this, &ChannelListPanel::lockChannel);
    connect(m_join, &QAction::triggered, this, &ChannelListPanel::joinChannel);
    connect(m_admin, &QAction::triggered, this, &ChannelListPanel::logAsAdmin);
    connect(m_channelPassword, &QAction::triggered, this, &ChannelListPanel::setPasswordOnChannel);
    connect(m_resetChannel, &QAction::triggered, this, &ChannelListPanel::resetChannel);
    connect(m_moveUserToCurrentChannel, &QAction::trigger, this, &ChannelListPanel::moveUserToCurrent);
}

ChannelListPanel::~ChannelListPanel()
{
    delete ui;
}
void ChannelListPanel::processMessage(NetworkMessageReader* msg)
{
    switch(msg->action())
    {
    case NetMsg::Goodbye:
        break;
    case NetMsg::Kicked:
        break;
    case NetMsg::MoveChannel:
        break;
    case NetMsg::SetChannelList:
    {
        QByteArray data= msg->byteArray32();
        QJsonDocument doc= QJsonDocument::fromJson(data);
        if(!doc.isEmpty())
        {
            QJsonObject obj= doc.object();
            m_model->readDataJson(obj);
        }
    }
    break;
    case NetMsg::GMStatus:
    {
        auto isGM= msg->uint8();
        if(isGM && (m_currentGroups == VIEWER))
        {
            setCurrentGroups(m_currentGroups | GAMEMASTER);
        }
    }
    break;
    case NetMsg::AdminAuthFail:
        setCurrentGroups(VIEWER);
        break;
    case NetMsg::AdminAuthSucessed:
        setCurrentGroups(m_currentGroups | ADMIN);
        break;
    default:
        break;
    }
}

void ChannelListPanel::sendOffModel()
{
    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::SetChannelList);
    QJsonDocument doc;
    QJsonObject obj;
    m_model->writeDataJson(obj);
    doc.setObject(obj);

    msg.byteArray32(doc.toJson());
    msg.sendToServer();
}
void ChannelListPanel::showCustomMenu(QPoint pos)
{
    QMenu menu(this);
    enum ClickState
    {
        Out,
        OnChannel,
        OnCurrentChannel,
        OnUser
    };

    ClickState state= Out;
    bool isGmChannel= false;
    bool isCurrentChannel= false;
    bool isOwnUser= false;
    bool hasPassword= false;

    m_index= ui->m_channelView->indexAt(pos);

    if(!m_index.isValid())
    {
        state= Out;
    }
    else
    {
        TreeItem* data= indexToPointer<TreeItem*>(m_index);
        if(data->isLeaf())
        {
            state= OnUser;
            isOwnUser= (m_localPlayerId == data->getId());
        }
        else
        {
            state= OnChannel;

            auto channel= dynamic_cast<Channel*>(data);
            if(channel)
            {
                if(!channel->password().isEmpty())
                    hasPassword= true;
                auto child= channel->getChildById(m_localPlayerId);
                if(child != nullptr)
                {
                    isCurrentChannel= true;
                    if(isGM())
                        isGmChannel= true;
                }
            }
        }
    }

    if(state == OnChannel && !isCurrentChannel)
    {
        menu.addAction(m_join);
        menu.addSeparator();
    }

    if(isGM() && isGmChannel)
    {
        menu.addAction(m_lock);
        menu.addAction(m_addSubchannel);
        menu.addSeparator();
    }

    if(((isGM() && isGmChannel) || isAdmin()) && (state == OnChannel))
    {
        menu.addAction(m_edit);
        menu.addAction(m_resetChannel);
        menu.addAction(m_deleteChannel);
        menu.addSeparator();
    }

    if(isAdmin())
    {
        if(state == OnChannel)
        {
            menu.addAction(m_setDefault);
            menu.addAction(m_channelPassword);
        }
        else if(state == OnUser && !isOwnUser)
        {
            menu.addAction(m_kick);
            // menu.addAction(m_ban);
        }
        menu.addAction(m_addChannel);
    }
    else
    {
        menu.addSeparator();
        menu.addAction(m_admin);
    }
    menu.exec(ui->m_channelView->mapToGlobal(pos));
}
bool ChannelListPanel::isAdmin()
{
    return (ChannelListPanel::ADMIN & m_currentGroups);
}

bool ChannelListPanel::isGM()
{
    return (ChannelListPanel::GAMEMASTER & m_currentGroups);
}

void ChannelListPanel::kickUser()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            TcpClient* item= getClient(m_index);
            if(item == nullptr)
                return;

            QString id= item->getId();
            QString idPlayer= item->getPlayerId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::Kicked);
                msg.string8(id);
                msg.string8(idPlayer);
                msg.sendToServer();
            }
        }
    }
}
void ChannelListPanel::lockChannel()
{
    if(!isGM() || !m_index.isValid())
        return;

    Channel* item= getChannel(m_index);
    if(item == nullptr)
        return;
    QString id= item->getId();
    if(!id.isEmpty())
    {
        auto action= item->locked() ? NetMsg::UnlockChannel : NetMsg::LockChannel;
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, action);
        msg.string8(id);
        msg.sendToServer();
    }
}

template <typename T>
T ChannelListPanel::indexToPointer(QModelIndex index)
{
    T item= static_cast<T>(index.internalPointer());
    return item;
}

TcpClient* ChannelListPanel::getClient(QModelIndex index)
{
    auto item= indexToPointer<TreeItem*>(index);
    if(item->isLeaf())
    {
        return static_cast<TcpClient*>(index.internalPointer());
    }
    return nullptr;
}

Channel* ChannelListPanel::getChannel(QModelIndex index)
{
    auto item= indexToPointer<TreeItem*>(index);
    if(!item->isLeaf())
    {
        return static_cast<Channel*>(index.internalPointer());
    }
    return nullptr;
}

void ChannelListPanel::banUser()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            TcpClient* item= getClient(m_index); /// static_cast<TcpClient*>(m_index.internalPointer());
            QString id= item->getId();
            QString idPlayer= item->getPlayerId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::BanUser);
                msg.string8(id);
                msg.string8(idPlayer);
                msg.sendToServer();
            }
        }
    }
}

void ChannelListPanel::logAsAdmin()
{
    PreferencesManager* preferences= PreferencesManager::getInstance();

    QString pwadmin= preferences->value(QString("adminPassword_for_%1").arg(m_serverName), QString()).toString();

    if(pwadmin.isEmpty())
    {
        pwadmin= QInputDialog::getText(this, tr("Admin Password"), tr("Password"), QLineEdit::Password);
    }
    auto pwA= QCryptographicHash::hash(pwadmin.toUtf8(), QCryptographicHash::Sha3_512);
    sendOffLoginAdmin(pwA);
}

void ChannelListPanel::sendOffLoginAdmin(QByteArray str)
{
    if(!str.isEmpty())
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AdminPassword);
        // msg.string8(id);
        msg.byteArray32(str);
        msg.sendToServer();
    }
}
void ChannelListPanel::addChannel()
{
    if(isAdmin())
    {
        Channel* newChannel= new Channel(tr("New Channel"));

        Channel* parent= getChannel(m_index);

        QModelIndex justAdded= m_model->addChannelToIndex(newChannel, m_index);
        ui->m_channelView->edit(justAdded);

        if(nullptr != parent)
        {
            QString parentId= parent->getId();
            if(!parentId.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AddChannel);
                msg.string8(parentId);
                newChannel->fill(msg);
                msg.sendToServer();
            }
        }
    }
}
void ChannelListPanel::addChannelAsSibbling()
{
    if(isAdmin())
    {
        Channel* newChannel= new Channel(tr("New Channel"));

        auto parentIndex= m_index.parent();

        Channel* parent= nullptr;
        QString parentId("");

        if(parentIndex.isValid())
        {
            parent= getChannel(parentIndex);
            parentId= parent->getId();
        }

        QModelIndex justAdded= m_model->addChannelToIndex(newChannel, parentIndex);
        ui->m_channelView->edit(justAdded);

        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::AddChannel);
        msg.string8(parentId);
        newChannel->fill(msg);
        msg.sendToServer();
    }
}

void ChannelListPanel::editChannel()
{
    auto chan= getChannel(m_index);
    if(!chan)
        return;
    bool rightToEdit= isAdmin();

    if(!rightToEdit && isGM())
    {
        if(nullptr != chan->getChildById(m_localPlayerId))
            rightToEdit= true;
    }

    if(rightToEdit)
    {
        ui->m_channelView->edit(m_index);
    }
}

QString ChannelListPanel::serverName() const
{
    return m_serverName;
}

void ChannelListPanel::setServerName(const QString& serverName)
{
    m_serverName= serverName;
}

void ChannelListPanel::setLocalPlayerId(const QString& id)
{
    m_localPlayerId= id;
    m_model->setLocalPlayerId(id);
}

void ChannelListPanel::resetChannel()
{
    if(!m_index.isValid())
        return;

    if(isAdmin() || isGM())
    {
        Channel* item= getChannel(m_index);
        if(item == nullptr)
            return;

        QString id= item->getId();

        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ResetChannel);
        msg.string8(id);
        msg.sendToServer();
    }
}

void ChannelListPanel::moveUserToCurrent()
{
    if(!m_index.isValid())
        return;

    auto subject= static_cast<TreeItem*>(m_index.internalPointer());

    if(!subject->isLeaf())
        return;

    if(isAdmin() || isGM())
    {
        auto local= m_model->getTcpClientById(m_localPlayerId);
        if(nullptr == local)
            return;

        auto channel= local->getParentChannel();

        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::MoveChannel);
        msg.string8(subject->getId());
        msg.string8(channel->getId());
        msg.sendToServer();
    }
}

void ChannelListPanel::deleteChannel()
{
    if(isAdmin())
    {
        if(m_index.isValid())
        {
            Channel* item= getChannel(m_index);
            if(nullptr == item)
                return;
            QString id= item->getId();
            if(!id.isEmpty())
            {
                NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::DeleteChannel);
                msg.string8(id);
                msg.sendToServer();
            }
        }
    }
}

void ChannelListPanel::setPasswordOnChannel()
{
    if(!isAdmin() && !m_index.isValid())
        return;

    Channel* item= getChannel(m_index);

    if(nullptr == item)
        return;

    auto pw= QInputDialog::getText(this, tr("Channel Password"),
        tr("Password for channel: %1 - leave empty for no password").arg(item->getName()), QLineEdit::Password,
        item->password());

    if(pw.isEmpty())
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ResetChannelPassword);
        msg.string8(item->getId());
        msg.sendToServer();
        return;
    }

    NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::ChannelPassword);
    msg.string8(item->getId());
    auto pwA= QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha3_512);
    msg.byteArray32(pwA);
    msg.sendToServer();
}

void ChannelListPanel::joinChannel()
{
    if(!m_index.isValid())
        return;

    Channel* item= getChannel(m_index);
    if(nullptr == item)
        return;

    QByteArray pw;
    if(!item->password().isEmpty())
        pw= QInputDialog::getText(
            this, tr("Channel Password"), tr("Channel %1 required password:").arg(item->getName()), QLineEdit::Password)
                .toUtf8();

    QString id= item->getId();
    if(!id.isEmpty())
    {
        NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::JoinChannel);
        msg.string8(id);
        msg.string8(m_localPlayerId);
        auto pwA= QCryptographicHash::hash(pw, QCryptographicHash::Sha3_512);
        msg.byteArray32(pwA);
        msg.sendToServer();
    }
}

ChannelListPanel::Groups ChannelListPanel::currentGroups() const
{
    return m_currentGroups;
}

void ChannelListPanel::setCurrentGroups(const Groups& currentGroups)
{
    m_currentGroups= currentGroups;
}
void ChannelListPanel::cleanUp()
{
    if(nullptr != m_model)
        m_model->cleanUp();
}
