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

#include "mediacontrollerbase.h"
#include "model/participantmodel.h"
#include <core_global.h>
class ParticipantModel;
class PlayerModel;
class Player;
class CORE_EXPORT SharedNoteController : public MediaControllerBase
{
    Q_OBJECT
    Q_PROPERTY(PlayerModel* playerModel READ playerModel NOTIFY playerModelChanged)
    Q_PROPERTY(ParticipantModel::Permission permission READ permission WRITE setPermission NOTIFY permissionChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(bool participantPanelVisible READ participantPanelVisible WRITE setParticipantPanelVisible NOTIFY
                   participantPanelVisibleChanged)
    Q_PROPERTY(HighlightedSyntax highligthedSyntax READ highligthedSyntax WRITE setHighligthedSyntax NOTIFY
                   highligthedSyntaxChanged)
    Q_PROPERTY(ParticipantModel* participantModel READ participantModel CONSTANT)
    Q_PROPERTY(bool markdownVisible READ markdownVisible WRITE setMarkdownVisible NOTIFY markdownVisibleChanged)
    Q_PROPERTY(QString textUpdate READ textUpdate WRITE setTextUpdate NOTIFY textUpdateChanged)
    Q_PROPERTY(QString updateCmd READ updateCmd WRITE setUpdateCmd NOTIFY updateCmdChanged)
public:
    enum class HighlightedSyntax : quint8
    {
        None,
        MarkDown
    };
    Q_ENUM(HighlightedSyntax)

    SharedNoteController(const QString& ownerId= QString(), const QString& local= QString(),
                         const QString& uuid= QString(), QObject* parent= nullptr);
    ~SharedNoteController() override;

    QString text() const;
    ParticipantModel* participantModel() const;
    ParticipantModel::Permission permission() const;
    bool participantPanelVisible() const;
    HighlightedSyntax highligthedSyntax() const;
    bool markdownVisible() const;

    PlayerModel* playerModel() const;

    bool canWrite(Player* player) const;
    bool canRead(Player* player) const;
    bool localCanWrite() const;
    QString textUpdate() const;
    QString updateCmd() const;
    static void setPlayerModel(PlayerModel* model);

public slots:
    void setPermission(ParticipantModel::Permission);
    void setText(const QString& text);
    void setHighligthedSyntax(SharedNoteController::HighlightedSyntax syntax);
    void setParticipantPanelVisible(bool b);

    void demoteCurrentItem(const QModelIndex& index);
    void promoteCurrentItem(const QModelIndex& index);

    void setMarkdownVisible(bool b);
    void setTextUpdate(const QString& cmd);
    void setUpdateCmd(const QString& cmd);

signals:
    void playerModelChanged();
    void permissionChanged(ParticipantModel::Permission);
    void textChanged(QString str);
    void highligthedSyntaxChanged();
    void participantPanelVisibleChanged(bool);
    void markdownVisibleChanged(bool);

    void openShareNoteTo(QString id);
    void closeShareNoteTo(QString id);
    void updateCmdChanged();

    void userCanWrite(QString id, bool);
    void textUpdateChanged(QString);
    void collabTextChanged(int pos, int charsRemoved, int charsAdded, QString tmpCmd);

private:
    std::unique_ptr<ParticipantModel> m_participantModel;
    static QPointer<PlayerModel> m_playerModel;
    QString m_text;
    QString m_latestCommand;
    HighlightedSyntax m_highlightedSyntax= HighlightedSyntax::MarkDown;
    bool m_participantVisible= true;
    bool m_markdownPreview= false;
};

#endif // SHAREDNOTECONTROLLER_H
