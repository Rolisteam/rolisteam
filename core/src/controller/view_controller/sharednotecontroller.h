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
#ifndef SHAREDNOTECONTROLLER_H
#define SHAREDNOTECONTROLLER_H

#include <QAbstractItemModel>
#include <QObject>
#include <QPointer>
#include <memory>

#include "abstractmediacontroller.h"

class ParticipantModel;
class PlayerModel;
class Player;
class SharedNoteController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(PlayerModel* playerModel READ playerModel NOTIFY playerModelChanged)
    Q_PROPERTY(Permission permission READ permission WRITE setPermission NOTIFY permissionChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool participantPanelVisible READ participantPanelVisible WRITE setParticipantPanelVisible NOTIFY
                   participantPanelVisibleChanged)
    Q_PROPERTY(HighlightedSyntax highligthedSyntax READ highligthedSyntax WRITE setHighligthedSyntax NOTIFY
                   highligthedSyntaxChanged)
    Q_PROPERTY(ParticipantModel* participantModel READ participantModel CONSTANT)
    Q_PROPERTY(bool markdownVisible READ markdownVisible WRITE setMarkdownVisible NOTIFY markdownVisibleChanged)
    Q_PROPERTY(QString textUpdate READ textUpdate WRITE setTextUpdate NOTIFY textUpdateChanged)
public:
    enum class Permission : char
    {
        NONE,
        READ,
        READWRITE
    };
    Q_ENUMS(Permission)

    enum class HighlightedSyntax : char
    {
        None,
        MarkDown
    };
    Q_ENUM(HighlightedSyntax)

    SharedNoteController(PlayerModel* model, CleverURI* uri, QObject* parent= nullptr);
    ~SharedNoteController() override;

    QString text() const;
    ParticipantModel* participantModel() const;
    Permission permission() const;
    bool participantPanelVisible() const;
    HighlightedSyntax highligthedSyntax() const;
    bool markdownVisible() const;

    void saveData() const override;
    void loadData() const override;

    PlayerModel* playerModel() const;

    bool canWrite(Player* player) const;
    bool canRead(Player* player) const;
    bool localCanWrite() const;
    QString textUpdate() const;

public slots:
    void setPermission(Permission);
    void setText(const QString& text);
    void setHighligthedSyntax(SharedNoteController::HighlightedSyntax syntax);
    void setParticipantPanelVisible(bool b);

    void demoteCurrentItem(const QModelIndex& index);
    void promoteCurrentItem(const QModelIndex& index);

    void setMarkdownVisible(bool b);
    void setTextUpdate(const QString& cmd);

    void runUpdateCmd(const QString& cmd);
signals:
    void playerModelChanged();
    void permissionChanged(Permission);
    void textChanged(QString str);
    void highligthedSyntaxChanged();
    void participantPanelVisibleChanged(bool);
    void markdownVisibleChanged(bool);

    void openShareNoteTo(QString id);
    void closeShareNoteTo(QString id);

    void userCanWrite(QString id, bool);
    void textUpdateChanged(QString);
    void partialChangeOnText(QString text);
    void collabTextChanged(int pos, int charsRemoved, int charsAdded, QString tmpCmd);

private:
    std::unique_ptr<ParticipantModel> m_participantModel;
    QPointer<PlayerModel> m_playerModel;
    QString m_text;
    QString m_latestCommand;
    Permission m_permission= Permission::NONE;
    HighlightedSyntax m_highlightedSyntax= HighlightedSyntax::MarkDown;
    bool m_participantVisible= true;
    bool m_markdownPreview= false;
};

#endif // SHAREDNOTECONTROLLER_H
