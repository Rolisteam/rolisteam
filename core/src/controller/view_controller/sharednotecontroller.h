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

class PlayerModel;
class SharedNoteController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(PlayerModel* playerModel READ playerModel NOTIFY playerModelChanged)
    Q_PROPERTY(Permission permission READ permission WRITE setPermission NOTIFY permissionChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(HighlightedSyntax highligthedSyntax READ highligthedSyntax WRITE setHighligthedSyntax NOTIFY
                   highligthedSyntaxChanged)
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
    Permission permission() const;
    HighlightedSyntax highligthedSyntax() const;

    void saveData() const override;
    void loadData() const override;

    PlayerModel* playerModel() const;

public slots:
    void setPermission(Permission);
    void setText(const QString& text);
    void setHighligthedSyntax(SharedNoteController::HighlightedSyntax syntax);

signals:
    void playerModelChanged();
    void permissionChanged(Permission);
    void textChanged(QString str);
    void highligthedSyntaxChanged();

private:
    QPointer<PlayerModel> m_model;
    QString m_text;
    Permission m_permission= Permission::NONE;
    HighlightedSyntax m_highlightedSyntax= HighlightedSyntax::MarkDown;
};

#endif // SHAREDNOTECONTROLLER_H
