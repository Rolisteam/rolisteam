/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef CHARACTERSHEETMODEL_H
#define CHARACTERSHEETMODEL_H

#include <QAbstractItemModel>

#include <QFile>
#include <QPointF>
#include <QPointer>
#include <QTextStream>

#include <charactersheet/charactersheet_global.h>

#include "charactersheetitem.h"

class CharacterSheet;
class Section;
class CSItem;

namespace Formula
{
class FormulaManager;
}
/**
 * @brief CharacterSheetModel is part of the MVC architecture for charactersheet viewer. it herits from
 * QAbstractItemModel it also provides features for adding data into stored CharacterSheet.
 */
class CHARACTERSHEET_EXPORT CharacterSheetModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum CustomRole
    {
        FormulaRole= Qt::UserRole + 1,
        ValueRole,
        UuidRole,
        NameRole
    };
    Q_ENUM(CustomRole)
    /**
     * @brief default constructor
     */
    CharacterSheetModel();
    virtual ~CharacterSheetModel();

    int rowCount(const QModelIndex& parent= QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex& parent= QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index) const;
    int columnCount(const QModelIndex& parent= QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool hasChildren(const QModelIndex& parent) const;

    void addSection(/*int index*/);
    void addLine(const QModelIndex& index);
    TreeSheetItem* indexToSection(const QModelIndex& index);
    QModelIndex indexToSectionIndex(const QModelIndex& index);
    CharacterSheet* getCharacterSheet(int id) const;

    bool writeModel(QJsonObject& file);
    void readModel(const QJsonObject& file, bool readRootSection);

    TreeSheetItem* addSection(QString title);
    void addLine(TreeSheetItem* parentItem, QString name, const QModelIndex& parent);

    void setRootSection(Section* rootSection);
    Section* getRootSection() const;

    void addCharacterSheet(CharacterSheet* sheet, int pos= -1);
    CharacterSheet* getCharacterSheetById(const QString& id) const;
    int getCharacterSheetCount() const;
    void removeCharacterSheet(int index);
    void removeCharacterSheet(CharacterSheet* sheet);
    void releaseCharacterSheet(CharacterSheet* sheet);

    CharacterSheet* addCharacterSheet();

    QList<CharacterSheet*> sheets() const;
public slots:
    void clearModel();

    void checkCharacter(Section* section);
    void addSubChildRoot(TreeSheetItem* item);
    void fieldHasBeenChanged(CharacterSheet* sheet, CSItem* item, const QString&);
    void addSubChild(CharacterSheet* sheet, CSItem* item);

signals:
    void characterSheetHasBeenAdded(CharacterSheet* sheet);
    void dataCharacterChange();
    void checkIntegrity();

protected:
    void computeFormula(QString path, CharacterSheet* sheet);
    void fieldUpdated(CharacterSheet* sheet, const QString& id);

private:
    void checkTableItem();
    TreeSheetItem* getProperItem(int row, int column, TreeSheetItem* field) const;

private:
    std::vector<std::unique_ptr<CharacterSheet>> m_characterList;
    QPointer<Section> m_rootSection;
    std::unique_ptr<Formula::FormulaManager> m_formulaManager;
};

#endif // CHARACTERSHEETMODEL_H
