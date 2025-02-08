/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "controller/instantmessagingcontroller.h"

#include <QJSEngine>
#include <QQmlEngine>
#include <QQuickStyle>

#include "common/logcategory.h"
#include "common_qml/theme.h"
#include "data/chatroom.h"
#include "data/player.h"
#include "diceparser_qobject/diceroller.h"
#include "instantmessaging/textwritercontroller.h"
#include "model/chatroomsplittermodel.h"
#include "model/filterinstantmessagingmodel.h"
#include "model/instantmessagingmodel.h"
#include "model/messagemodel.h"
#include <QFont>

#include "updater/media/instantmessagingupdater.h"

void registerType()
{
    // TODO Move that into view? or define module with Qt6/cmake syntax
    static bool registered= false;

    if(registered)
        return;

    qDebug() << "registerType 2";
    qRegisterMetaType<InstantMessaging::FilterInstantMessagingModel*>("FilterInstantMessagingModel*");
    qRegisterMetaType<InstantMessaging::MessageModel*>("MessageModel*");
    qRegisterMetaType<InstantMessaging::FilteredPlayerModel*>("FilteredPlayerModel*");
    qRegisterMetaType<LocalPersonModel*>("LocalPersonModel*");
    qRegisterMetaType<PlayerModel*>("PlayerModel*");
    qRegisterMetaType<InstantMessaging::ChatRoom*>("ChatRoom*");
    qRegisterMetaType<customization::Theme*>("customization::Theme*");
    qRegisterMetaType<customization::StyleSheet*>("customization::StyleSheet*");
    qRegisterMetaType<InstantMessaging::ChatroomSplitterModel*>("ChatroomSplitterModel*");

    qmlRegisterAnonymousType<InstantMessagingController>("InstantMessagingController", 1);
    qmlRegisterAnonymousType<InstantMessaging::FilterInstantMessagingModel>("FilterInstantMessagingModel", 1);
    qmlRegisterType<InstantMessaging::FilteredPlayerModel>("IMModels", 1, 0, "FilteredPlayerModel");
    qmlRegisterAnonymousType<InstantMessaging::MessageModel>("MessageModel", 1);
    qmlRegisterAnonymousType<LocalPersonModel>("LocalPersonModel", 1);
    qmlRegisterAnonymousType<PlayerModel>("PlayerModel", 1);
    qmlRegisterAnonymousType<InstantMessaging::ChatroomSplitterModel>("ChatroomSplitterModel", 1);
    qmlRegisterUncreatableType<InstantMessaging::ChatRoom>("InstantMessaging", 1, 0, "ChatRoom",
                                                           "ChatRoom can't be created.");
    qmlRegisterType<InstantMessaging::TextWriterController>("InstantMessaging", 1, 0, "TextWriterController");
    qmlRegisterSingletonType<customization::Theme>("Customization", 1, 0, "Theme",
                                                   [](QQmlEngine* engine, QJSEngine*) -> QObject*
                                                   {
                                                       auto instead= customization::Theme::instance();
                                                       engine->setObjectOwnership(instead, QQmlEngine::CppOwnership);
                                                       return instead;
                                                   });

    qmlRegisterUncreatableType<InstantMessaging::MessageInterface>("Customization", 1, 0, "MessageInterface",
                                                                   "Not creatable as it is an enum type.");

    registered= true;
}

InstantMessagingController::InstantMessagingController(DiceRoller* diceRoller, PlayerModel* model, QObject* parent)
    : AbstractControllerInterface(parent)
    , m_localPersonModel(new LocalPersonModel)
    , m_model(new InstantMessaging::InstantMessagingModel(diceRoller, model))
    , m_players(model)
    , m_diceParser(diceRoller)
    , m_font(QFont().toString())
{
    registerType();
    addChatroomSplitterModel();

    m_localPersonModel->setSourceModel(m_players);

    connect(m_model.get(), &InstantMessaging::InstantMessagingModel::unreadChanged, this,
            &InstantMessagingController::unreadChanged);
    connect(m_model.get(), &InstantMessaging::InstantMessagingModel::chatRoomCreated, this,
            &InstantMessagingController::chatRoomCreated);
    connect(m_model.get(), &InstantMessaging::InstantMessagingModel::chatRoomDeleted, this,
            &InstantMessagingController::chatRoomRemoved);
    connect(m_model.get(), &InstantMessaging::InstantMessagingModel::localIdChanged, this,
            &InstantMessagingController::localIdChanged);
    connect(m_players, &PlayerModel::playerJoin, this,
            [this](Player* player)
            {
                if(nullptr == player)
                    return;
                if(player->uuid() == localId())
                    return;

                emit playerArrived(player->uuid());

                if(m_model->hasInvidualChatroom({player->uuid(), localId()}))
                    return;

                m_model->insertIndividualChatroom(player->uuid(), player->name());
            });

    connect(m_players, &PlayerModel::playerLeft, this,
            [this](Player* player)
            {
                if(nullptr == player)
                    return;

                m_model->removePlayer(player->uuid());
            });

    m_model->insertGlobalChatroom(tr("Global"), QStringLiteral("global"));
}

InstantMessagingController::~InstantMessagingController()= default;

InstantMessaging::ChatroomSplitterModel* InstantMessagingController::mainModel() const
{
    Q_ASSERT(m_splitterModels.size() > 0);
    return m_splitterModels[0].get();
}

InstantMessaging::ChatRoom* InstantMessagingController::globalChatroom() const
{
    return m_model->globalChatRoom();
}

InstantMessaging::ChatRoom* InstantMessagingController::gmChatroom() const
{
    if(!m_players)
        return nullptr;
    return m_model->oneToOneChatRoom(m_players->gameMasterId());
}

PlayerModel* InstantMessagingController::playerModel() const
{
    return m_players;
}

LocalPersonModel* InstantMessagingController::localPersonModel() const
{
    return m_localPersonModel.get();
}

QString InstantMessagingController::localId() const
{
    return m_model->localId();
}

bool InstantMessagingController::nightMode() const
{
    return m_nightMode;
}

bool InstantMessagingController::visible() const
{
    return m_visible;
}

bool InstantMessagingController::unread() const
{
    return m_model->unread();
}

void InstantMessagingController::openLink(const QString& link)
{
    qCDebug(MessagingCat) << "open link" << link;
    emit openWebPage(link);
}

void InstantMessagingController::setDiceParser(DiceRoller* diceParser)
{
    m_model->setDiceParser(diceParser);
}

void InstantMessagingController::setVisible(bool b)
{
    if(b == m_visible)
        return;
    m_visible= b;
    emit visibleChanged(m_visible);
}

void InstantMessagingController::rollDiceCommand(const QString& cmd, bool gmOnly, const QString& characterId)
{
    auto chatRoom= gmOnly ? gmChatroom() : globalChatroom();
    if(!chatRoom)
    {
        qCWarning(MessagingCat) << "Error: no " << (gmOnly ? "Gm" : "Global") << " chatroom";
        return;
    }
    chatRoom->rollDice(cmd, characterId);
}

void InstantMessagingController::sendMessageToGlobal(const QString& msg, const QString& characterId)
{
    auto chatRoom= globalChatroom();
    if(!chatRoom)
    {
        qCWarning(MessagingCat) << "Error: no Gm chatroom";
        return;
    }
    auto person= m_players->personById(characterId);
    QString name;
    if(person)
        name= person->name();
    chatRoom->addMessage(msg, {}, characterId, name);
}

void InstantMessagingController::sendMessageToGM(const QString& msg, const QString& characterId)
{
    auto chatRoom= gmChatroom();
    if(!chatRoom)
    {
        qCWarning(MessagingCat) << "Error: no Gm chatroom";
        return;
    }
    auto person= m_players->personById(characterId);
    QString name;
    if(person)
        name= person->name();
    chatRoom->addMessage(msg, {}, characterId, name);
}

void InstantMessagingController::setGameController(GameController*) {}

void InstantMessagingController::closeChatroom(const QString& id, bool network)
{
    m_model->removeChatroom(id, network);
}

void InstantMessagingController::detach(const QString& id, int index)
{
    Q_UNUSED(index);
    Q_UNUSED(id);
}

void InstantMessagingController::reattach(const QString& id, int index)
{
    Q_UNUSED(index);
    Q_UNUSED(id);
}

void InstantMessagingController::splitScreen(const QString& id, int index)
{
    if(index >= static_cast<int>(m_splitterModels.size()) || index < 0)
        return;

    auto model= m_splitterModels.at(static_cast<std::size_t>(index)).get();
    model->addFilterModel(m_model.get(), {id}, false);
}

void InstantMessagingController::setLocalId(const QString& id)
{
    m_model->setLocalId(id);
}

void InstantMessagingController::addExtraChatroom(const QString& title, bool everyone, const QVariantList& recipiant)
{
    if(everyone)
        m_model->insertGlobalChatroom(title);
    else
    {
        QStringList ids;
        ids.reserve(recipiant.size());
        std::transform(recipiant.begin(), recipiant.end(), std::back_inserter(ids),
                       [](const QVariant& variant) { return variant.toString(); });

        m_model->insertExtraChatroom(title, ids, false);
    }
}

void InstantMessagingController::setNightMode(bool mode)
{
    if(m_nightMode == mode)
        return;
    m_nightMode= mode;
    emit nightModeChanged(m_nightMode);
}

void InstantMessagingController::addChatroomSplitterModel()
{
    std::unique_ptr<InstantMessaging::ChatroomSplitterModel> model(new InstantMessaging::ChatroomSplitterModel);
    model->addFilterModel(m_model.get());
    m_splitterModels.push_back(std::move(model));
}

void InstantMessagingController::translateDiceResult(const QHash<int, QList<int>>& rollResult, const QString& rest)
{
    if(rollResult.isEmpty())
        return;
    QString cmd;

    for(auto [k, v] : rollResult.asKeyValueRange())
    {
        QStringList list;
        std::transform(std::begin(v), std::end(v), std::back_inserter(list), [](int i) { return QString::number(i); });

        cmd+= QString("[%1];").arg(list.join(","));
    }

    cmd.removeLast();
    cmd.append(rest);

    rollDiceCommand(cmd, false, localId());
}

InstantMessaging::InstantMessagingModel* InstantMessagingController::model() const
{
    return m_model.get();
}

bool InstantMessagingController::sound() const
{
    return m_sound;
}

void InstantMessagingController::setSound(bool newSound)
{
    if(m_sound == newSound)
        return;
    m_sound= newSound;
    emit soundChanged();
}

QFont InstantMessagingController::font() const
{
    return m_font;
}

void InstantMessagingController::setFont(const QFont& newFont)
{
    if(m_font == newFont)
        return;
    m_font= newFont;
    emit fontChanged();
}

int InstantMessagingController::currentTab() const
{
    return m_currentTab;
}

void InstantMessagingController::setCurrentTab(int newCurrentTab)
{
    if(m_currentTab == newCurrentTab)
        return;
    m_currentTab= newCurrentTab;
    emit currentTabChanged();
}
