#include "channellistpanel.h"
#include "ui_channellistpanel.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>

#include "network/channel.h"
#include "network/serverconnection.h"
#include "preferences/preferencesmanager.h"

ChannelListPanel::ChannelListPanel(PreferencesManager* preferences, NetworkController* ctrl, QWidget* parent)
    : QWidget(parent), ui(new Ui::ChannelListPanel), m_ctrl(ctrl), m_pref(preferences)
{
    ui->setupUi(this);
    ui->m_channelView->setModel(m_ctrl->channelModel());
    ui->m_channelView->setAlternatingRowColors(true);
    ui->m_channelView->setHeaderHidden(true);
    ui->m_channelView->setAcceptDrops(true);
    ui->m_channelView->setDragEnabled(true);
    ui->m_channelView->setDropIndicatorShown(true);
    ui->m_channelView->setDefaultDropAction(Qt::MoveAction);
    ui->m_channelView->setDragDropMode(QAbstractItemView::InternalMove);

    ui->m_channelView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->m_channelView, &QTreeView::customContextMenuRequested, this, &ChannelListPanel::showCustomMenu);
    connect(m_ctrl->channelModel(), &ChannelModel::localPlayerGMChanged, this,
            &ChannelListPanel::CurrentChannelGmIdChanged);

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
    connect(m_moveUserToCurrentChannel, &QAction::triggered, this, &ChannelListPanel::moveUserToCurrent);
}

ChannelListPanel::~ChannelListPanel()
{
    delete ui;
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

    m_index= ui->m_channelView->indexAt(pos);

    if(!m_index.isValid())
    {
        state= Out;
    }
    else
    {
        TreeItem* dataItem= indexToPointer<TreeItem*>(m_index);
        if(dataItem->isLeaf())
        {
            state= OnUser;
            isOwnUser= (m_localPlayerId == dataItem->uuid());
        }
        else
        {
            state= OnChannel;

            auto channel= dynamic_cast<Channel*>(dataItem);
            if(channel)
            {
                /*if(!channel->password().isEmpty())
                    hasPassword= true;*/
                auto child= channel->getChildById(m_localPlayerId);
                if(child != nullptr)
                {
                    isCurrentChannel= true;
                    if(m_ctrl->isGM())
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

    if(m_ctrl->isGM() && isGmChannel)
    {
        menu.addAction(m_lock);
        menu.addAction(m_addSubchannel);
        menu.addSeparator();
    }

    if(((m_ctrl->isGM() && isGmChannel) || m_ctrl->isAdmin()) && (state == OnChannel))
    {
        menu.addAction(m_edit);
        menu.addAction(m_resetChannel);
        menu.addAction(m_deleteChannel);
        menu.addSeparator();
    }

    if(m_ctrl->isAdmin())
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

void ChannelListPanel::kickUser()
{
    if(!m_ctrl->isAdmin())
        return;

    if(!m_index.isValid())
        return;

    ServerConnection* item= getClient(m_index);
    if(item == nullptr)
        return;

    QString id= item->uuid();
    QString idPlayer= item->playerId();
    m_ctrl->kickUser(id, idPlayer);
}
void ChannelListPanel::lockChannel()
{
    if(!m_ctrl->isGM() || !m_index.isValid())
        return;

    Channel* item= getChannel(m_index);
    if(item == nullptr)
        return;
    QString id= item->uuid();
    if(id.isEmpty())
        return;

    auto action= item->locked() ? NetMsg::UnlockChannel : NetMsg::LockChannel;
    m_ctrl->lockChannel(id, action);
}

template <typename T>
T ChannelListPanel::indexToPointer(QModelIndex index)
{
    T item= static_cast<T>(index.internalPointer());
    return item;
}

ServerConnection* ChannelListPanel::getClient(QModelIndex index)
{
    auto item= indexToPointer<TreeItem*>(index);
    if(item->isLeaf())
    {
        return static_cast<ServerConnection*>(index.internalPointer());
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
    if(!m_ctrl->isAdmin() || !m_index.isValid())
        return;

    ServerConnection* item= getClient(m_index); /// static_cast<ServerConnection*>(m_index.internalPointer());
    if(!item)
        return;

    QString id= item->uuid();
    QString idPlayer= item->playerId();

    if(id.isEmpty())
        return;

    m_ctrl->banUser(id, idPlayer);
}

void ChannelListPanel::logAsAdmin()
{
    QString pwadmin;

    if(m_pref)
        pwadmin= m_pref->value(QString("adminPassword_for_%1").arg(m_serverName), QString()).toString();

    if(pwadmin.isEmpty())
        pwadmin= QInputDialog::getText(this, tr("Admin Password"), tr("Password"), QLineEdit::Password);

    m_ctrl->sendOffLoginAdmin(pwadmin);
}

void ChannelListPanel::addChannel()
{
    /*  if(isAdmin())
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
      }*/
}
void ChannelListPanel::addChannelAsSibbling()
{
    /*  if(isAdmin())
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
      }*/
}

void ChannelListPanel::editChannel()
{
    auto chan= getChannel(m_index);
    if(!chan)
        return;
    bool rightToEdit= m_ctrl->isGM();

    if(!rightToEdit && m_ctrl->isGM())
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
}

void ChannelListPanel::resetChannel()
{
    if(!m_index.isValid())
        return;

    if(m_ctrl->groups() == NetworkController::VIEWER)
        return;

    Channel* item= getChannel(m_index);
    if(item == nullptr)
        return;

    QString id= item->uuid();

    m_ctrl->resetChannel(id);
}

void ChannelListPanel::moveUserToCurrent()
{
    if(!m_index.isValid())
        return;

    auto subject= static_cast<TreeItem*>(m_index.internalPointer());

    if(!subject->isLeaf())
        return;

    // if(isAdmin() || isGM())
    {
        /*    auto local= m_model->getServerConnectionById(m_localPlayerId);
            if(nullptr == local)
                return;

            auto channel= local->getParentChannel();

            NetworkMessageWriter msg(NetMsg::AdministrationCategory, NetMsg::MoveChannel);
            msg.string8(subject->getId());
            msg.string8(channel->getId());
            msg.sendToServer();*/
    }
}

void ChannelListPanel::deleteChannel()
{
    if(!m_index.isValid())
        return;

    Channel* item= getChannel(m_index);
    if(nullptr == item)
        return;
    QString id= item->uuid();

    m_ctrl->deleteChannel(id);
}

void ChannelListPanel::setPasswordOnChannel()
{
    if(!m_ctrl->isAdmin() && !m_index.isValid())
        return;

    Channel* item= getChannel(m_index);

    if(nullptr == item)
        return;

    auto pw= QInputDialog::getText(this, tr("Channel Password"),
                                   tr("Password for channel: %1 - leave empty for no password").arg(item->name()),
                                   QLineEdit::Password, item->password());

    auto pwA= pw.isEmpty() ? QByteArray() : QCryptographicHash::hash(pw.toUtf8(), QCryptographicHash::Sha3_512);
    m_ctrl->definePasswordOnChannel(item->uuid(), pwA);
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
        pw= QInputDialog::getText(this, tr("Channel Password"), tr("Channel %1 required password:").arg(item->name()),
                                  QLineEdit::Password)
                .toUtf8();

    QString id= item->uuid();
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

void ChannelListPanel::cleanUp()
{
    /*   if(nullptr != m_model)
           m_model->cleanUp();*/
}
