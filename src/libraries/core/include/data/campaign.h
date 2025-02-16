/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include <QObject>
#include <QString>
#include <memory>

#include "data/rolisteamtheme.h"
#include "media/mediatype.h"
#include <core_global.h>

#include "model/characterstatemodel.h"
#include "model/dicealiasmodel.h"
#include "model/nonplayablecharactermodel.h"

class DiceAliasModel;
class CharacterStateModel;
namespace campaign
{
class Media;
class NonPlayableCharacterModel;
constexpr auto TRASH_FOLDER{".trash"};
constexpr auto MODEL_FILE{"data.json"};
constexpr auto THEME_FILE{"theme.json"};
constexpr auto FIRST_AUDIO_PLAYER_FILE{"audioplayer1.json"};
constexpr auto SECOND_AUDIO_PLAYER_FILE{"audioplayer2.json"};
constexpr auto THIRD_AUDIO_PLAYER_FILE{"audioplayer3.json"};
constexpr auto DICE_ALIAS_MODEL{"dice_command.json"};
constexpr auto STATE_MODEL{"states.json"};
constexpr auto MEDIA_ROOT{"media"};
constexpr auto IM_DATA{"chatrooms.json"};
constexpr auto STATE_ROOT{"states"};
constexpr auto STATIC_ROOT{"static"};
constexpr auto CHARACTER_ROOT{"npcs"};
constexpr auto CHARACTER_MODEL{"npc.json"};
constexpr auto DICE3D_CONTROLLER{"3dDice.json"};
constexpr auto CONTENTS_FILE{"content.json"};

class CORE_EXPORT Campaign : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString rootDirectory READ rootDirectory WRITE setRootDirectory NOTIFY rootDirectoryChanged)
    Q_PROPERTY(QString currentChapter READ currentChapter WRITE setCurrentChapter NOTIFY currentChapterChanged)
    Q_PROPERTY(DiceAliasModel* diceAliases READ diceAliases NOTIFY diceAliasesChanged)
    Q_PROPERTY(CharacterStateModel* stateModel READ stateModel NOTIFY stateModelChanged)
    Q_PROPERTY(NonPlayableCharacterModel* npcModel READ npcModel NOTIFY npcModelChanged)
    Q_PROPERTY(RolisteamTheme* currentTheme READ currentTheme WRITE setCurrentTheme NOTIFY currentThemeChanged)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(qint64 diskUsage READ diskUsage NOTIFY diskUsageChanged)
    Q_PROPERTY(int fileCount READ fileCount NOTIFY fileCountChanged)
    Q_PROPERTY(bool loadSession READ loadSession WRITE setLoadSession NOTIFY loadSessionChanged FINAL)
public:
    explicit Campaign(QObject* parent= nullptr);
    enum class State : quint8
    {
        None,
        Checking,
        Ready
    };
    Q_ENUM(State);
    enum class Move
    {
        UP,
        DOWN,
        BOTTOM,
        TOP
    };
    enum class Place : quint8
    {
        MEDIA_ROOT,
        STATIC_ROOT,
        CONTENT_ROOT,
        STATE_ROOT,
        STATE_MODEL,
        TRASH_ROOT,
        NPC_ROOT,
        NPC_MODEL,
        THEME_FILE,
        IM_FILE,
        DICE_MODEL,
        FIRST_AUDIO_PLAYER_FILE,
        SECOND_AUDIO_PLAYER_FILE,
        THIRD_AUDIO_PLAYER_FILE,
        DICE_3D_FILE
    };
    Q_ENUM(Place);

    virtual ~Campaign();

    QString name() const;
    QString rootDirectory() const;
    QString currentChapter() const;
    DiceAliasModel* diceAliases() const;
    CharacterStateModel* stateModel() const;
    RolisteamTheme* currentTheme() const;
    NonPlayableCharacterModel* npcModel() const;
    State state() const;
    qint64 diskUsage() const;
    int fileCount() const;

    QString currentStorePath() const;

    void addMedia(std::unique_ptr<Media>&& media);
    void removeMedia(const QString& id);
    const std::vector<std::unique_ptr<Media>>& medias() const;
    Media* mediaFromPath(const QString& path) const;
    Media* mediaFromUuid(const QString& uuid) const;
    QString pathFromUuid(const QString& uuid) const;

    QString directory(Place place) const;

    bool loadSession() const;
    void setLoadSession(bool newLoadSession);

public slots:
    void setRootDirectory(const QString& root);
    void setCurrentChapter(const QString& chapter);
    void setCurrentTheme(RolisteamTheme* themeuri);
    void setName(const QString& name);
    void postError(const QString& msg);
    void renameMedia(const QString& id, const QString& name);

    // alias
    void addAlias();
    void deleteAlias(const QModelIndex& index);
    void moveAlias(const QModelIndex& index, campaign::Campaign::Move move);
    QString convertAlias(const QString& str);

    // states
    void addState();
    void deleteState(const QModelIndex& index);
    void moveState(const QModelIndex& index, campaign::Campaign::Move move);

    // Characters
    void addCharacter();
    void removeCharacter(const QString& id);

private slots:
    void setState(campaign::Campaign::State state);

signals:
    void rootDirectoryChanged();
    void currentChapterChanged();

    // models
    void diceAliasesChanged();
    void stateModelChanged();
    void npcModelChanged();

    void currentThemeChanged();
    void stateChanged();
    void nameChanged();
    void errorOccured(QString msg);
    void mediaAdded(campaign::Media* media);
    void mediaNameChanged(const QString& id, const QString& path);
    void mediaRemoved(const QString& id);
    void diskUsageChanged();
    void fileCountChanged();

    void loadSessionChanged();

private:
    std::vector<std::unique_ptr<campaign::Media>> m_mediaList;
    std::unique_ptr<DiceAliasModel> m_diceModel;
    std::unique_ptr<CharacterStateModel> m_stateModel;
    std::unique_ptr<campaign::NonPlayableCharacterModel> m_npcModel;
    QString m_name;
    QString m_root;
    QString m_currentChapter;
    std::unique_ptr<RolisteamTheme> m_theme;
    State m_state= State::None;
    bool m_loadSession{false};
};
} // namespace campaign
#endif // CAMPAIGN_H
