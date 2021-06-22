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
#ifndef NONPLAYABLECHARACTERMODEL_H
#define NONPLAYABLECHARACTERMODEL_H

#include "data/character.h"

#include <QAbstractListModel>
#include <QPointer>

namespace campaign
{
class NonPlayableCharacter : public Character
{
    Q_OBJECT
    Q_PROPERTY(QString tokenPath READ tokenPath WRITE setTokenPath NOTIFY tokenPathChanged)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
public:
    NonPlayableCharacter(QObject* parent= nullptr);

    QString tokenPath() const;
    QStringList tags() const;

public slots:
    void setTokenPath(const QString& path);
    void setTags(const QStringList& list);

signals:
    void tokenPathChanged();
    void tagsChanged();

private:
    QString m_tokenPath;
    QStringList m_tags;
};

class NonPlayableCharacterModel : public QAbstractListModel
{
    Q_OBJECT
    /*
     * tr("Name") << tr("Color") << tr("rtoken") << tr("Min HP") << tr("Current HP")
                 << tr("Max HP") << tr("Initiative") << tr("Distance Per turn") << tr("State") << tr("Life Color")
                 << tr("Init Command");*/
public:
    enum CustomRole
    {
        RoleName= Qt::UserRole + 1,
        RoleColor,
        RoleTokenPath,
        RoleMinHp,
        RoleCurrentHp,
        RoleMaxHp,
        RoleInitiative,
        RoleDistancePerTurn,
        RoleState,
        RoleLifeColor,
        RoleInitCommand,
        RoleTags,
        RoleAvatar,
        RoleUuid,
        RoleUnknown
    };
    Q_ENUM(CustomRole)
    enum Columns
    {
        ColName,
        ColColor,
        ColTokenPath,
        ColMinHp,
        ColCurrentHp,
        ColMaxHp,
        ColInitiative,
        ColDistancePerTurn,
        ColState,
        ColLifeColor,
        ColInitCommand,
        ColTags,
        ColAvatar,
        ColUnknown
    };
    Q_ENUM(Columns)
    explicit NonPlayableCharacterModel(QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex&) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void append();
    void addCharacter(NonPlayableCharacter* character);
    void setModelData(const std::vector<NonPlayableCharacter*>& data);
    void removeNpc(const QModelIndex& index);
    void clear();

    const std::vector<std::unique_ptr<NonPlayableCharacter>>& npcList() const;

signals:
    void characterAdded();
    void characterRemoved(QString id);

private:
    std::vector<std::unique_ptr<NonPlayableCharacter>> m_data;
    QStringList m_header;
};
} // namespace campaign
#endif // NONPLAYABLECHARACTERMODEL_H
