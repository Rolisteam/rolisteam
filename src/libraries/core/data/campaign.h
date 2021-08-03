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

#include "media/mediatype.h"

class DiceAliasModel;
class CharacterStateModel;
class RolisteamTheme;
namespace campaign
{
class Media;
class NonPlayableCharacterModel;
constexpr char const* TRASH_FOLDER{".trash"};
constexpr char const* MODEL_FILE{"data.json"};
constexpr char const* THEME_FILE{"theme.json"};
constexpr char const* DICE_ALIAS_MODEL{"dice_command.json"};
constexpr char const* STATE_MODEL{"states.json"};
constexpr char const* MEDIA_ROOT{"media"};
constexpr char const* STATE_ROOT{"states"};
constexpr char const* CHARACTER_ROOT{"npcs"};
constexpr char const* CHARACTER_MODEL{"npc.json"};

class Campaign : public QObject
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
        STATE_ROOT,
        TRASH_ROOT,
        NPC_ROOT
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

    QString currentStorePath() const;

    void addMedia(std::unique_ptr<Media> media);
    void removeMedia(const QString& id);
    const std::vector<std::unique_ptr<Media>>& medias() const;
    Media* mediaFromPath(const QString& path) const;
    Media* mediaFromUuid(const QString& uuid) const;

    QString directory(Place place) const;

public slots:
    void setRootDirectory(const QString& root);
    void setCurrentChapter(const QString& chapter);
    void setCurrentTheme(RolisteamTheme* themeuri);
    void setName(const QString& name);
    void postError(const QString& msg);

    // alias
    void addAlias();
    void deleteAlias(const QModelIndex& index);
    void moveAlias(const QModelIndex& index, campaign::Campaign::Move move);
    QString convertAlias(const QString& str);

    // states
    void addState();
    void deleteState(const QModelIndex& index);
    void moveState(const QModelIndex& index, Move move);

    // Characters
    void addCharacter();
    void removeCharacter(const QString& id);

private slots:
    void setState(State state);

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
    void mediaAdded(Media* media);
    void mediaRemoved(const QString& id);

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
};
} // namespace campaign
#endif // CAMPAIGN_H
