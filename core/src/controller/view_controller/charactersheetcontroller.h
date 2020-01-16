/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   renaud@rolisteam.org                                     *
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
#ifndef CHARACTERSHEETCONTROLLER_H
#define CHARACTERSHEETCONTROLLER_H

#include <QAbstractItemModel>
#include <QJsonObject>
#include <QPointer>
#include <memory>
#include <set>

#include "abstractmediacontroller.h"

class Player;
class Character;
class CharacterSheet;
class CharacterModel;
struct CharacterSheetData
{
    CharacterSheet* sheet;
    Player* player;
    Character* character;
};

class ImageModel;
class CharacterSheetModel;
class CharacterSheetItem;
class CharacterSheetController : public AbstractMediaContainerController
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(QAbstractItemModel* imageModel READ imageModel CONSTANT)
    Q_PROPERTY(QAbstractItemModel* characterModel READ characterModel CONSTANT)
    Q_PROPERTY(QString qmlCode READ qmlCode CONSTANT)
    Q_PROPERTY(bool cornerEnabled READ cornerEnabled NOTIFY cornerEnabledChanged)
public:
    CharacterSheetController(CharacterModel* characterModel, CleverURI* uri, QObject* parent= nullptr);
    ~CharacterSheetController() override;

    virtual void saveData() const override;
    virtual void loadData() const override;

    QAbstractItemModel* model() const;
    QAbstractItemModel* imageModel() const;
    QAbstractItemModel* characterModel() const;

    QString qmlCode() const;
    bool cornerEnabled() const;

signals:
    void cornerEnabledChanged(bool);

private:
    void updateFieldFrom(CharacterSheet* sheet, CharacterSheetItem* item, const QString& parentPath);

private:
    std::unique_ptr<CharacterSheetModel> m_model;
    std::unique_ptr<ImageModel> m_imageModel;
    QPointer<CharacterModel> m_characterModel;
    std::set<CharacterSheetData> m_sheetData;
    QJsonObject m_rootJson;
    QString m_qmlCode;
};

#endif // CHARACTERSHEETCONTROLLER_H
