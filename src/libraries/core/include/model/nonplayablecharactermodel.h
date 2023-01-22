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
#include "model/characterstatemodel.h"

#include <QAbstractListModel>
#include <QPointer>
#include <core_global.h>

namespace campaign
{
class CORE_EXPORT NonPlayableCharacter : public Character
{
    Q_OBJECT
    Q_PROPERTY(QString avatarPath READ avatarPath WRITE setAvatarPath NOTIFY avatarPathChanged)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags NOTIFY tagsChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString gameMasterDesc READ gameMasterDesc WRITE setGameMasterDesc NOTIFY gameMasterDescChanged)
public:
    NonPlayableCharacter(QObject* parent= nullptr);

    QStringList tags() const;
    QString avatarPath() const;
    int size() const;
    QString description() const;

    const QString& gameMasterDesc() const;
    void setGameMasterDesc(const QString& newGameMasterDesc);

public slots:
    void setTags(const QStringList& list);
    void setAvatarPath(const QString& path);
    void setSize(int size);
    void setDescription(const QString& desc);

signals:
    void tagsChanged();
    void avatarPathChanged();
    void sizeChanged();
    void descriptionChanged();

    void gameMasterDescChanged();

private:
    QString m_avatarPath;
    QStringList m_tags;
    int m_size;
    QString m_description;
    QString m_gameMasterDesc;
};

class CORE_EXPORT NonPlayableCharacterModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString npcRoot READ npcRoot WRITE setNpcRoot NOTIFY npcRootChanged)
public:
    enum CustomRole
    {
        RoleAvatar= Qt::UserRole + 1,
        RoleName,
        RoleDescription,
        RoleGmDetails,
        RoleTags,
        RoleColor,
        RoleMinHp,
        RoleCurrentHp,
        RoleMaxHp,
        RoleInitiative,
        RoleDistancePerTurn,
        RoleState,
        RoleLifeColor,
        RoleInitCommand,
        RoleUuid,
        RoleAvatarPath,
        RoleHasInitiative,
        RoleActionCount,
        RoleShapeCount,
        RolePropertiesCount,
        RoleUnknown
    };
    Q_ENUM(CustomRole)
    enum Columns
    {
        ColAvatar,
        ColName,
        ColDescription,
        ColGmDetails,
        ColTags,
        ColColor,
        ColMinHp,
        ColCurrentHp,
        ColMaxHp,
        ColInitiative,
        ColDistancePerTurn,
        ColState,
        ColLifeColor,
        ColInitCommand,
        ColUnknown
    };
    Q_ENUM(Columns)
    explicit NonPlayableCharacterModel(CharacterStateModel* states, QObject* parent= nullptr);

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
    void removeNpc(const QString& index);
    void refresh(const QString& index);
    void clear();

    const std::vector<std::unique_ptr<NonPlayableCharacter>>& npcList() const;
    QStringList headers();

    QModelIndex indexFromUuid(const QString& id);
    NonPlayableCharacter* characterFromUuid(const QString& id);

    const QString& npcRoot() const;
    void setNpcRoot(const QString& newNpcRoot);

private:
    QString stateName(const QString& id) const;

signals:
    void characterAdded();
    void characterRemoved(QString id);
    void npcRootChanged();

private:
    std::vector<std::unique_ptr<NonPlayableCharacter>> m_data;
    QPointer<CharacterStateModel> m_states;
    QStringList m_header;
    QString m_npcRoot;
};
} // namespace campaign
#endif // NONPLAYABLECHARACTERMODEL_H
