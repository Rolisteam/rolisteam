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
#ifndef CAMPAIGN_ANTAGONISTBOARDCONTROLLER_H
#define CAMPAIGN_ANTAGONISTBOARDCONTROLLER_H

#include <QObject>
#include <QPointer>

#include <array>
#include <memory>

#include "core_global.h"
#include "model/characterstatemodel.h"
#include "model/colormodel.h"
#include "model/filteredcharactermodel.h"
#include "model/genericmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "model/statemodel.h"

namespace campaign
{
Q_NAMESPACE
class CampaignEditor;

enum class ModelType
{
    Model_Action,
    Model_Shape,
    Model_Properties
};
CORE_EXPORT Q_ENUM_NS(ModelType);

class CORE_EXPORT AntagonistBoardController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(GenericModel* actionModel READ actionModel CONSTANT)
    Q_PROPERTY(GenericModel* shapeModel READ shapeModel CONSTANT)
    Q_PROPERTY(GenericModel* propertyModel READ propertyModel CONSTANT)
    Q_PROPERTY(StateModel* stateModel READ stateModel NOTIFY stateModelChanged)
    Q_PROPERTY(NonPlayableCharacter* character READ character WRITE setCharacter NOTIFY characterChanged)
    Q_PROPERTY(ColorModel* colorModel READ colorModel CONSTANT)
    Q_PROPERTY(StateModel* stateModel READ stateModel CONSTANT)
    Q_PROPERTY(bool editingCharacter READ editingCharacter NOTIFY characterChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(FilteredCharacterModel* filteredModel READ filteredModel CONSTANT)
    Q_PROPERTY(QString imageFolder READ imageFolder CONSTANT)
    Q_PROPERTY(CharacterStateModel* characterStateModel READ characterStateModel NOTIFY characterStateModelChanged)
public:
    explicit AntagonistBoardController(campaign::CampaignEditor* editor, QObject* parent= nullptr);

    GenericModel* actionModel() const;
    GenericModel* shapeModel() const;
    GenericModel* propertyModel() const;
    QString searchText() const;
    FilteredCharacterModel* filteredModel() const;
    QString imageFolder() const;

    bool editingCharacter() const;
    NonPlayableCharacter* character() const;

    ColorModel* colorModel() const;
    StateModel* stateModel() const;
    CharacterStateModel* characterStateModel() const;

public slots:
    void removeData(const QModelIndex& index, campaign::ModelType type);
    void appendData(campaign::ModelType type);
    void setSearchText(const QString& text);
    void editCharacter(const QString& id);
    void setCharacter(campaign::NonPlayableCharacter* character);
    void saveToken();

    void addCharacter();
    void removeCharacter(const QString& id);
    void changeImage(const QString& id, const QByteArray& filename);

signals:
    void characterChanged();
    void searchTextChanged();
    void characterEditionChanged();
    void stateModelChanged();
    void characterStateModelChanged();

private:
    QPointer<campaign::CampaignEditor> m_editor;
    std::array<GenericModel*, 3> m_models;
    QPointer<NonPlayableCharacter> m_character;
    std::unique_ptr<FilteredCharacterModel> m_filteredModel;
    std::unique_ptr<ColorModel> m_colorModel;
    std::unique_ptr<StateModel> m_stateModel;
    QString m_imageFolder;
    int m_size= 64;
};
} // namespace campaign
#endif // CAMPAIGN_ANTAGONISTBOARDCONTROLLER_H
