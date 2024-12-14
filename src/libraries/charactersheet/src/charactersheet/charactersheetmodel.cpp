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

#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/charactersheet.h"
#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/section.h"
#include "charactersheet/controllers/tablefield.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <charactersheet_formula/formulamanager.h>
#include <core/include/media/mediatype.h>

/////////////////////////////
/// CharacterSheetModel
/////////////////////////////

CharacterSheetModel::CharacterSheetModel()
    : m_rootSection(new Section()), m_formulaManager(new Formula::FormulaManager())
{
}
CharacterSheetModel::~CharacterSheetModel()= default;

int CharacterSheetModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return m_rootSection->childrenCount();
    if(parent.column() > 0)
        return 0;
    int val= 0;

    TreeSheetItem* tmp= static_cast<TreeSheetItem*>(parent.internalPointer());

    if(tmp->itemType() == TreeSheetItem::TableItem && !m_characterList.empty())
    {
        int max= tmp->childrenCount();
        auto result
            = std::max_element(std::begin(m_characterList), std::end(m_characterList),
                               [tmp](const std::unique_ptr<CharacterSheet>& a, const std::unique_ptr<CharacterSheet>& b)
                               {
                                   auto fieldA= a->getFieldFromKey(tmp->id());
                                   auto fieldB= b->getFieldFromKey(tmp->id());
                                   if(!fieldA || !fieldB)
                                       return false;
                                   return fieldA->childrenCount() < fieldB->childrenCount();
                               });

        auto maxfield= (*result)->getFieldFromKey(tmp->id());
        if(maxfield)
            val= std::max(max, maxfield->childrenCount());
    }
    else if(tmp)
        val= tmp->childrenCount();
    else
        qDebug() << "parent is valid but internal pointer is not";

    qDebug() << "result:" << parent << val;

    return val;
}
CharacterSheet* CharacterSheetModel::getCharacterSheet(int id) const
{
    if((!m_characterList.empty()) && (m_characterList.size() > static_cast<unsigned int>(id)) && (0 <= id))
    {
        return m_characterList[id].get();
    }
    return nullptr;
}

int CharacterSheetModel::columnCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;

    return m_characterList.size() + 1;
}
QModelIndex CharacterSheetModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();

    TreeSheetItem* childItem= nullptr;

    if(!parent.isValid() && column == 0) // column of fields
    {
        childItem= m_rootSection->childAt(row);
    }
    else if(column > 0 && !parent.isValid()) // column of charactersheet
    {
        auto item= m_rootSection->childAt(row);
        auto sheet= getCharacterSheet(column - 1);
        if(item && sheet)
        {
            auto path= item->path();
            childItem= sheet->getFieldFromKey(path);
        }
    }
    else if(parent.isValid()) // subsection
    {
        auto parentItem= static_cast<TreeSheetItem*>(parent.internalPointer());
        if(parentItem)
            childItem= parentItem->childAt(row);
    }
    return childItem ? createIndex(row, column, childItem) : QModelIndex();
}
QModelIndex CharacterSheetModel::parent(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();

    TreeSheetItem* childItem= static_cast<TreeSheetItem*>(index.internalPointer());

    if(nullptr == childItem)
        return QModelIndex();

    TreeSheetItem* parentItem= childItem->parentTreeItem();

    if(parentItem == m_rootSection.get() || parentItem == nullptr)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->rowInParent(), 0, parentItem);
}

QVariant CharacterSheetModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if((role == Qt::TextAlignmentRole) && (index.column() != 0))
        return Qt::AlignHCenter;

    static QSet<int> roles{Qt::DisplayRole, Qt::EditRole, Qt::BackgroundRole, Qt::ToolTipRole, Qt::UserRole,
                           FormulaRole,     ValueRole,    UuidRole,           NameRole};

    QVariant var;
    if(!roles.contains(role))
        return {};

    auto childItem= static_cast<TreeSheetItem*>(index.internalPointer());
    if(nullptr == childItem)
        return {};

    if(role == Qt::BackgroundRole && index.column() != 0)
    {

        QString path= childItem->path();
        CharacterSheet* sheet= getCharacterSheet(index.column() - 1);
        bool isReadOnly= sheet->getValue(path, Qt::BackgroundRole).toBool();
        if(isReadOnly)
        {
            var= QColor(128, 128, 128);
        }
    }
    else if(role != Qt::BackgroundRole)
    {
        if(index.column() == 0)
        {
            var= childItem->valueFrom(TreeSheetItem::ColumnId::LABEL, Qt::DisplayRole);
        }
        else
        {
            if(childItem->itemType() == TreeSheetItem::CellValue)
            {
                switch(role)
                {
                case Qt::DisplayRole:
                case Qt::EditRole:
                    var= childItem->valueFrom(TreeSheetItem::VALUE, role);
                    break;
                case UuidRole:
                case NameRole:
                case Qt::ToolTipRole:
                    var= childItem->valueFrom(TreeSheetItem::ID, role);
                    break;
                }
            }
            else
            {
                QString path= childItem->path();
                CharacterSheet* sheet= getCharacterSheet(index.column() - 1);
                if(role == UuidRole)
                    var= childItem->valueFrom(TreeSheetItem::ID, role);
                else if(role == NameRole)
                    var= childItem->valueFrom(TreeSheetItem::LABEL, role);
                else
                    var= sheet->getValue(path, static_cast<Qt::ItemDataRole>(role));
            }
        }
    }

    return var;
}

bool CharacterSheetModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid() || Qt::EditRole != role)
        return false;

    TreeSheetItem* childItem= static_cast<TreeSheetItem*>(index.internalPointer());

    if(nullptr == childItem)
        return false;

    auto valueStr= value.toString();
    QString formula;
    CharacterSheet* sheet= getCharacterSheet(index.column() - 1);
    if(valueStr.startsWith('=') && sheet)
    {
        formula= valueStr;
        QHash<QString, QString> hash= sheet->getVariableDictionnary();
        m_formulaManager->setConstantHash(hash);
        valueStr= m_formulaManager->getValue(formula).toString();
        childItem->setFormula(formula);
    }

    TreeSheetItem::ColumnId columnId= index.column() == 0 ? TreeSheetItem::LABEL : TreeSheetItem::VALUE;

    if(childItem->itemType() == TreeSheetItem::CellValue && index.column() > 0)
    {
        auto parentItem= dynamic_cast<TreeSheetItem*>(childItem->parentTreeItem());
        auto parent= dynamic_cast<TableFieldController*>(parentItem);
        Q_ASSERT(parent->itemType() == TreeSheetItem::TableItem);
        auto model= parent->model();
        model->setData(model->indexFromCell(childItem), valueStr, role);
    }
    else if(index.column() > 0)
    {
        QString path= childItem->path();

        CSItem* newitem= sheet->setValue(path, valueStr, formula);
        if(nullptr != newitem)
        {
            newitem->setValueFrom(TreeSheetItem::LABEL, childItem->valueFrom(TreeSheetItem::LABEL, Qt::DisplayRole));
            newitem->setOrig(childItem);
        }
        computeFormula(path, sheet);
    }

    childItem->setValueFrom(columnId, valueStr);
    emit dataCharacterChange();

    return false;
}
void CharacterSheetModel::computeFormula(QString path, CharacterSheet* sheet)
{
    QStringList List= sheet->getAllDependancy(path);

    for(auto& item : List)
    {
        QString formula;
        QString valueStr;

        QHash<QString, QString> hash= sheet->getVariableDictionnary();
        m_formulaManager->setConstantHash(hash);
        formula= sheet->getValue(item, Qt::EditRole).toString();
        valueStr= m_formulaManager->getValue(formula).toString();
        sheet->setValue(item, valueStr, formula);
    }
}

void CharacterSheetModel::fieldUpdated(CharacterSheet* sheet, const QString& id)
{
    auto it
        = std::find_if(std::begin(m_characterList), std::end(m_characterList),
                       [sheet](const std::unique_ptr<CharacterSheet>& sheetData) { return sheetData.get() == sheet; });

    if(it == std::end(m_characterList))
        return;

    auto c= std::distance(std::begin(m_characterList), it) + 1;

    auto r= sheet->indexFromId(id);

    emit dataChanged(index(r, c), index(r, c), {Qt::EditRole, ValueRole});
}

void CharacterSheetModel::fieldHasBeenChanged(CharacterSheet* sheet, CSItem* item, const QString&)
{
    emit dataCharacterChange();
    computeFormula(item->label(), sheet);
}

void CharacterSheetModel::clearModel()
{
    beginResetModel();
    m_characterList.clear();
    if(nullptr != m_rootSection)
    {
        m_rootSection->removeAll();
    }
    endResetModel();
}

void CharacterSheetModel::checkCharacter(Section* section)
{
    for(auto const& sheet : m_characterList)
    {
        for(int i= 0; i < section->childrenCount(); ++i)
        {
            auto id= dynamic_cast<CSItem*>(section->childAt(i));
            if(!id)
                continue;

            auto field= sheet->getFieldFromKey(id->id());
            if(nullptr == field)
            {
                if(id->fieldType() != FieldController::TABLE)
                {
                    FieldController* newField= new FieldController(TreeSheetItem::FieldItem, false);
                    newField->copyField(id, true);
                    sheet->insertCharacterItem(newField);
                    field= newField;
                }
                else // if(nullptr == field && id->fieldType() == FieldController::TABLE)
                {
                    auto newtablefield= new TableFieldController(false);
                    newtablefield->copyField(id, true);
                    sheet->insertCharacterItem(newtablefield);
                    field= newtablefield;
                }
            }
            for(int j= 0; j < id->childrenCount(); ++j)
            {
                auto childFormat= id->childAt(j);
                auto childCharacter= field->childAt(j);
                if(nullptr != childFormat && nullptr != childCharacter)
                {
                    if(childFormat->id() != childCharacter->id())
                    {
                        childCharacter->setId(childFormat->id());
                    }
                }
            }
            if(field->fieldType() != id->fieldType())
            {
                field->setFieldType(id->fieldType());
            }
            if(field->label() != id->label())
            {
                field->setLabel(id->label());
            }
        }
    }
}
void CharacterSheetModel::addCharacterSheet(CharacterSheet* sheet, int pos)
{
    if(pos < 0)
        pos= m_characterList.size();

    connect(sheet, &CharacterSheet::fieldValueChanged, this, &CharacterSheetModel::fieldUpdated);

    beginInsertColumns(QModelIndex(), pos + 1, pos + 1);
    std::unique_ptr<CharacterSheet> uSheet(sheet);
    m_characterList.insert(std::begin(m_characterList) + pos, std::move(uSheet));
    endInsertColumns();
    emit characterSheetHasBeenAdded(sheet);
    emit dataCharacterChange();
}

void CharacterSheetModel::addSubChildRoot(TreeSheetItem* item)
{
    if(!m_rootSection)
        return;

    auto parentItem= m_rootSection->childFromId(item->path());
    auto r= m_rootSection->indexOfChild(parentItem);
    auto structTable= dynamic_cast<TableFieldController*>(parentItem);
    if(structTable == nullptr)
        return;

    auto addedFieldCount= structTable->columnCount();
    auto index= createIndex(r, 0, parentItem);
    beginInsertRows(index, parentItem->childrenCount(), parentItem->childrenCount() + addedFieldCount);
    structTable->appendChild(nullptr);
    endInsertRows();
}

void CharacterSheetModel::addSubChild(CharacterSheet* sheet, CSItem* item)
{
    if(!m_rootSection)
        return;

    auto cit
        = std::find_if(std::begin(m_characterList), std::end(m_characterList),
                       [sheet](const std::unique_ptr<CharacterSheet>& dataSheet) { return dataSheet.get() == sheet; });

    if(cit == std::end(m_characterList))
        return;

    auto c= std::distance(std::begin(m_characterList), cit) + 1;

    auto parentItem= m_rootSection->childFromId(item->path());
    auto r= m_rootSection->indexOfChild(parentItem);
    auto table= dynamic_cast<TableFieldController*>(item);
    auto structTable= dynamic_cast<TableFieldController*>(parentItem);
    if(table == nullptr || structTable == nullptr)
        return;

    auto lineCount= table->rowCount();
    auto structLineCount= structTable->rowCount();
    auto addedFieldCount= table->columnCount();
    if(lineCount < 0 || structLineCount < 0 || addedFieldCount < 0)
        return;

    if(lineCount == structLineCount)
    {
        auto index= createIndex(r, 0, parentItem);
        beginInsertRows(index, parentItem->childrenCount(), parentItem->childrenCount() + addedFieldCount);
        structTable->appendChild(nullptr);
        endInsertRows();
    }

    auto index= createIndex(r, c, item);
    beginInsertRows(index, item->childrenCount(), item->childrenCount() + addedFieldCount);
    // generic method - for tableview the item is built from the last one.
    table->appendChild(nullptr);
    checkTableItem();
    endInsertRows();
}
void CharacterSheetModel::removeCharacterSheet(CharacterSheet* sheet)
{
    auto cit
        = std::find_if(std::begin(m_characterList), std::end(m_characterList),
                       [sheet](const std::unique_ptr<CharacterSheet>& dataSheet) { return dataSheet.get() == sheet; });

    if(cit == std::end(m_characterList))
        return;
    auto pos= std::distance(std::begin(m_characterList), cit);
    beginRemoveColumns(QModelIndex(), pos + 1, pos + 1);
    m_characterList.erase(cit);
    endRemoveColumns();
}

void CharacterSheetModel::releaseCharacterSheet(CharacterSheet* sheet)
{
    auto cit
        = std::find_if(std::begin(m_characterList), std::end(m_characterList),
                       [sheet](const std::unique_ptr<CharacterSheet>& dataSheet) { return dataSheet.get() == sheet; });

    if(cit == std::end(m_characterList))
        return;
    auto pos= std::distance(std::begin(m_characterList), cit);
    beginRemoveColumns(QModelIndex(), pos + 1, pos + 1);
    cit->release();
    m_characterList.erase(cit);
    endRemoveColumns();
}

void CharacterSheetModel::removeCharacterSheet(int index)
{
    beginRemoveColumns(QModelIndex(), index + 1, index + 1);
    m_characterList.erase(std::begin(m_characterList) + index);
    endRemoveColumns();
}

CharacterSheet* CharacterSheetModel::getCharacterSheetById(const QString& id) const
{
    auto it= std::find_if(std::begin(m_characterList), std::end(m_characterList),
                          [id](const std::unique_ptr<CharacterSheet>& dataSheet) { return dataSheet->uuid() == id; });

    if(it == std::end(m_characterList))
        return nullptr;
    else
        return (*it).get();
}

int CharacterSheetModel::getCharacterSheetCount() const
{
    return m_characterList.size();
}

Section* CharacterSheetModel::getRootSection() const
{
    return m_rootSection.get();
}

void CharacterSheetModel::setRootSection(Section* rootSection)
{
    auto previous= m_rootSection;

    beginResetModel();

    m_rootSection= rootSection;

    if(m_rootSection != previous)
        connect(m_rootSection.get(), &Section::addLineToTableField, this, &CharacterSheetModel::addSubChildRoot);

    for(auto const& character : m_characterList)
    {
        rootSection->buildDataInto(character.get());
    }
    endResetModel();
}

QVariant CharacterSheetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Fields name");
        default:
        {
            if(static_cast<int>(m_characterList.size()) > (section - 1))
            {
                auto character= getCharacterSheet(section - 1);
                return character->name();
            }
            else
            {
                return QString();
            }
        }
        }
    }
    return QVariant();
}

Qt::ItemFlags CharacterSheetModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    if(index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;

    auto* childItem= static_cast<TreeSheetItem*>(index.internalPointer());

    Qt::ItemFlags res;
    if(nullptr != childItem && childItem->readOnly())
        res= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        res= Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;

    return res;
}
void CharacterSheetModel::addSection()
{
    addSection(tr("Empty Section %1").arg(m_rootSection->childrenCount() + 1));
}
TreeSheetItem* CharacterSheetModel::addSection(QString title)
{
    beginInsertRows(QModelIndex(), m_rootSection->childrenCount(), m_rootSection->childrenCount());
    Section* rootSection= m_rootSection.get();
    Section* sec= new Section();
    sec->setId(tr("Section_%1").arg(title));
    rootSection->appendChild(sec);
    endInsertRows();
    emit dataCharacterChange();
    return sec;
}

void CharacterSheetModel::addLine(const QModelIndex& index)
{
    QModelIndex parent= index;
    TreeSheetItem* parentItem= nullptr;
    if(index.isValid())
    {
        parentItem= static_cast<TreeSheetItem*>(index.internalPointer());
    }
    else
    {
        parentItem= m_rootSection.get();
    }
    if(!parentItem->mayHaveChildren())
    {
        parentItem= parentItem->parentTreeItem();
        parent= parent.parent();
    }
    addLine(parentItem, tr("Field %1").arg(parentItem->childrenCount()), parent);
}
void CharacterSheetModel::addLine(TreeSheetItem* parentItem, QString name, const QModelIndex& parent)
{
    if(parentItem->mayHaveChildren())
    {
        beginInsertRows(parent, parentItem->childrenCount(), parentItem->childrenCount());
        Section* section= static_cast<Section*>(parentItem);
        FieldController* field= new FieldController(TreeSheetItem::FieldItem, true);
        field->setId(name.replace(' ', '_'));
        field->setLabel(name);
        section->appendChild(field);
        endInsertRows();
        emit dataCharacterChange();
    }
}
bool CharacterSheetModel::hasChildren(const QModelIndex& parent) const
{
    if(!parent.isValid()) // root
        return m_rootSection->childrenCount() > 0 ? true : false;
    else
    {
        TreeSheetItem* childItem= static_cast<TreeSheetItem*>(parent.internalPointer());
        if(nullptr == childItem)
            return false;

        if(childItem->childrenCount() == 0)
            return false;
        else
            return true;
    }
}
TreeSheetItem* CharacterSheetModel::indexToSection(const QModelIndex& index)
{
    if(index.isValid())
        return static_cast<TreeSheetItem*>(index.internalPointer());
    else
        return nullptr;
}
QModelIndex CharacterSheetModel::indexToSectionIndex(const QModelIndex& index)
{
    if(index.parent().isValid()) // if parent is valid it's a field, return its parent (the section).
        return index.parent();
    else
        return index;
}

bool CharacterSheetModel::writeModel(QJsonObject& jsonObj)
{
    jsonObj[Core::jsonctrl::sheet::JSON_CHARACTER_COUNT]= static_cast<int>(m_characterList.size()); // m_characterCount;

    QJsonArray characters;
    for(auto const& item : m_characterList)
    {
        QJsonObject charObj;
        item->save(charObj);
        characters.append(charObj);
    }
    jsonObj[Core::jsonctrl::sheet::JSON_CHARACTER_CONTENT]= characters;
    return true;
}

void CharacterSheetModel::readModel(const QJsonObject& jsonObj, bool readRootSection)
{
    beginResetModel();
    if(readRootSection)
    {
        QJsonObject data= jsonObj["data"].toObject();
        m_rootSection->load(data);
    }
    QJsonArray characters= jsonObj["characters"].toArray();
    for(const auto charJson : characters)
    {
        qDebug() << "read characterfrom json";
        QJsonObject obj= charJson.toObject();
        // CharacterSheet* sheet= new CharacterSheet();
        auto sheet= std::make_unique<CharacterSheet>();
        auto saved= sheet.get();
        sheet->load(obj);
        sheet->setOrigin(m_rootSection.get());
        m_characterList.push_back(std::move(sheet));
        emit characterSheetHasBeenAdded(saved);
    }
    checkTableItem();
    endResetModel();
}

void CharacterSheetModel::checkTableItem()
{
    for(int i= 0; i < m_rootSection->childrenCount(); ++i)
    {
        auto child= m_rootSection->childAt(i);
        if(TreeSheetItem::TableItem == child->itemType())
        {
            for(auto const& character : m_characterList)
            {
                auto childFromCharacter= character->getFieldAt(i);
                auto table= dynamic_cast<TableFieldController*>(child);
                if(table == nullptr)
                    return;
                while(childFromCharacter->childrenCount() > child->childrenCount())
                {
                    table->appendChild(nullptr);
                }
            }
        }
    }
}

CharacterSheet* CharacterSheetModel::addCharacterSheet()
{
    CharacterSheet* sheet= new CharacterSheet;
    m_rootSection->buildDataInto(sheet);
    addCharacterSheet(sheet, false);

    return sheet;
}

QList<CharacterSheet*> CharacterSheetModel::sheets() const
{
    QList<CharacterSheet*> res;
    std::transform(std::begin(m_characterList), std::end(m_characterList), std::back_inserter(res),
                   [](const std::unique_ptr<CharacterSheet>& sheet) { return sheet.get(); });
    return res;
}
