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
#include <QSortFilterProxyModel>
#include <memory>

class GenericModel;
namespace campaign
{
Q_NAMESPACE
class CampaignEditor;
class NonPlayableCharacter;
enum class ModelType
{
    Model_Action,
    Model_Shape,
    Model_Properties
};
Q_ENUM_NS(ModelType);

class FilteredCharacterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString search READ search WRITE setSearch NOTIFY searchChanged)
public:
    FilteredCharacterModel();
    QString search() const;
    void setSearch(const QString& search);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

signals:
    void searchChanged();

private:
    QString m_search;
    int m_role= -1;
};

class AntagonistBoardController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(GenericModel* actionModel READ actionModel CONSTANT)
    Q_PROPERTY(GenericModel* shapeModel READ shapeModel CONSTANT)
    Q_PROPERTY(GenericModel* propertyModel READ propertyModel CONSTANT)
    Q_PROPERTY(NonPlayableCharacter* character READ character WRITE setCharacter NOTIFY characterChanged)
    Q_PROPERTY(bool editingCharacter READ editingCharacter NOTIFY characterChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    Q_PROPERTY(FilteredCharacterModel* filteredModel READ filteredModel CONSTANT)
    Q_PROPERTY(QString imageFolder READ imageFolder CONSTANT)
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

public slots:
    void removeData(const QModelIndex& index, ModelType type);
    void appendData(ModelType type);
    void setSearchText(const QString& text);
    void editCharacter(const QString& id);
    void setCharacter(NonPlayableCharacter* character);

    void addCharacter();
    void removeCharacter(const QModelIndex& id);
    void changeImage(const QString& id, const QString& filename);
signals:
    void characterChanged();
    void searchTextChanged();

private:
    QPointer<campaign::CampaignEditor> m_editor;
    std::array<GenericModel*, 3> m_models;
    QPointer<NonPlayableCharacter> m_character;
    std::unique_ptr<FilteredCharacterModel> m_filteredModel;
    QString m_imageFolder;
};
} // namespace campaign
#endif // CAMPAIGN_ANTAGONISTBOARDCONTROLLER_H
