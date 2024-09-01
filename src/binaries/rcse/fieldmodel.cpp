/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#include "fieldmodel.h"

#include <QDebug>
#include <QGraphicsScene>
#include <QJsonArray>

#include "canvas.h"
#include "qmlgeneratorvisitor.h"
#include <charactersheet_formula/formulamanager.h>

//////////////////////////////
// Column
/////////////////////////////
Column::Column(QString name, TreeSheetItem::ColumnId pos) : m_name(name), m_pos(pos) {}

QString Column::getName() const
{
    return m_name;
}

void Column::setName(const QString& name)
{
    m_name= name;
}
TreeSheetItem::ColumnId Column::getPos() const
{
    return m_pos;
}

void Column::setPos(const TreeSheetItem::ColumnId& pos)
{
    m_pos= pos;
}

//////////////////////////////
// FieldModel
/////////////////////////////
FieldModel::FieldModel(QObject* parent)
    : QAbstractItemModel(parent), m_rootSection{new Section()}, m_formulaManager{new Formula::FormulaManager()}
{
    m_colunm << new Column(tr("Id"), TreeSheetItem::ID) << new Column(tr("Label"), TreeSheetItem::LABEL)
             << new Column(tr("Value"), TreeSheetItem::VALUE)
             << new Column(tr("Possible Values"), TreeSheetItem::VALUES) << new Column(tr("Type"), TreeSheetItem::TYPE)
             << new Column(tr("x"), TreeSheetItem::X) << new Column(tr("y"), TreeSheetItem::Y)
             << new Column(tr("Width"), TreeSheetItem::WIDTH) << new Column(tr("Height"), TreeSheetItem::HEIGHT)
             << new Column(tr("Font Adaptation"), TreeSheetItem::FitFont) << new Column(tr("Font"), TreeSheetItem::FONT)
             << new Column(tr("Text-align"), TreeSheetItem::TEXT_ALIGN)
             << new Column(tr("Text Color"), TreeSheetItem::TEXTCOLOR)
             << new Column(tr("Bg Color"), TreeSheetItem::BGCOLOR) << new Column(tr("Border"), TreeSheetItem::BORDER)
             << new Column(tr("Page"), TreeSheetItem::PAGE) << new Column(tr("ToolTip"), TreeSheetItem::TOOLTIP);

    m_alignList << tr("TopRight") << tr("TopMiddle") << tr("TopLeft") << tr("CenterRight") << tr("CenterMiddle")
                << tr("CenterLeft") << tr("BottomRight") << tr("BottomMiddle") << tr("BottomLeft");
}

FieldModel::~FieldModel()= default;

QVariant FieldModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();
    TreeSheetItem* treeitem= static_cast<TreeSheetItem*>(index.internalPointer());

    if(nullptr == treeitem)
        return {};

    auto item= dynamic_cast<CSItem*>(treeitem);

    if(nullptr == item)
    {
        if(index.column() == 0 && role == Qt::DisplayRole)
        {
            return item->id();
        }
        else
            return {};
    }
    if((role == Qt::DisplayRole) || (Qt::EditRole == role))
    {
        QVariant var;
        if(TreeSheetItem::VALUE == m_colunm[index.column()]->getPos() && role == Qt::EditRole)
        {

            auto formula= item->formula();
            var= formula.isEmpty() ? item->valueFrom(m_colunm[index.column()]->getPos(), role) : formula;
        }
        else
            var= item->valueFrom(m_colunm[index.column()]->getPos(), role);
        if((index.column() == TreeSheetItem::TEXT_ALIGN) && (Qt::DisplayRole == role))
        {
            if((var.toInt() >= 0) && (var.toInt() < m_alignList.size()))
            {
                var= m_alignList.at(var.toInt());
            }
        }
        return var;
    }
    if((role == Qt::BackgroundRole)
       && ((index.column() == TreeSheetItem::BGCOLOR) || (index.column() == TreeSheetItem::TEXTCOLOR)))
    {
        QVariant var= item->valueFrom(m_colunm[index.column()]->getPos(), Qt::EditRole);
        return var;
    }
    if(role == Qt::BackgroundRole)
    {
        auto field= dynamic_cast<FieldController*>(item);
        QVariant color;
        if(field && !field->generatedCode().isEmpty())
        {
            color= QColor(Qt::green).lighter();
        }
        if(field && field->isReadOnly() && (index.column() >= TreeSheetItem::X)
           && (index.column() <= TreeSheetItem::HEIGHT))
        {
            color= QColor(Qt::gray);
        }
        return color;
    }
    if((Qt::FontRole == role) && (index.column() == TreeSheetItem::FONT))
    {
        QVariant var= item->valueFrom(m_colunm[index.column()]->getPos(), Qt::DisplayRole);
        QFont font;
        font.fromString(var.toString());
        return font;
    }
    return QVariant();
}

bool FieldModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(!index.isValid() || Qt::EditRole != role)
        return false;

    TreeSheetItem* treeitem= static_cast<TreeSheetItem*>(index.internalPointer());

    if(nullptr == treeitem)
        return false;

    auto item= dynamic_cast<CSItem*>(treeitem);

    if(!item && index.column() == 0 && role == Qt::DisplayRole)
    {
        treeitem->setId(value.toString());
        return true;
    }
    if(nullptr == item)
        return false;

    auto valStr= value.toString();

    if(TreeSheetItem::VALUE == m_colunm[index.column()]->getPos() && valStr.startsWith("="))
    {
        QHash<QString, QString> hash= buildDicto();
        m_formulaManager->setConstantHash(hash);
        auto valueAfterComputation= m_formulaManager->getValue(valStr).toString();
        item->setFormula(valStr);
        item->setValueFrom(m_colunm[index.column()]->getPos(), valueAfterComputation);
    }
    else
    {
        item->setValueFrom(m_colunm[index.column()]->getPos(), value);
    }
    emit valuesChanged(item->valueFrom(TreeSheetItem::ID, Qt::DisplayRole).toString(), value.toString());
    emit modelChanged();
    return true;
}

QHash<QString, QString> FieldModel::buildDicto()
{
    QHash<QString, QString> dict;
    m_rootSection->setFieldInDictionnary(dict);
    return dict;
}

QModelIndex FieldModel::index(int row, int column, const QModelIndex& parent) const
{
    if(row < 0)
        return QModelIndex();

    TreeSheetItem* parentItem= nullptr;

    // qDebug()<< "Index session " <<row << column << parent;
    if(!parent.isValid())
        parentItem= m_rootSection.get();
    else
        parentItem= static_cast<TreeSheetItem*>(parent.internalPointer());

    TreeSheetItem* childItem= parentItem->childAt(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex FieldModel::parent(const QModelIndex& child) const
{
    if(!child.isValid())
        return QModelIndex();

    TreeSheetItem* childItem= static_cast<TreeSheetItem*>(child.internalPointer());
    TreeSheetItem* parentItem= childItem->parentTreeItem();

    if(parentItem == m_rootSection.get())
        return QModelIndex();

    TreeSheetItem* grandParent= parentItem->parentTreeItem();

    return createIndex(grandParent->indexOfChild(parentItem), 0, parentItem);
}

int FieldModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return m_rootSection->childrenCount();

    TreeSheetItem* childItem= static_cast<TreeSheetItem*>(parent.internalPointer());
    if(childItem)
        return childItem->childrenCount();
    else
        return 0;
}

int FieldModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_colunm.count();
}

QVariant FieldModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if((role == Qt::DisplayRole) && (orientation == Qt::Horizontal))
    {
        return m_colunm[section]->getName();
    }
    else
    {
        return QVariant();
    }
}

void FieldModel::appendField(CSItem* f)
{
    beginInsertRows(QModelIndex(), m_rootSection->childrenCount(), m_rootSection->childrenCount());
    m_rootSection->appendChild(f);
    auto func= [this, f]() { emit updateItem(f); };
    connect(f, &CSItem::characterSheetItemChanged, this, func);
    connect(f, &CSItem::xChanged, this, func);
    connect(f, &CSItem::yChanged, this, func);
    connect(f, &CSItem::widthChanged, this, func);
    connect(f, &CSItem::heightChanged, this, func);
    connect(f, &CSItem::borderChanged, this, func);
    connect(f, &CSItem::bgColorChanged, this, func);

    connect(f, &CSItem::valueChanged, this, func);
    connect(f, &CSItem::textColorChanged, this, func);
    connect(f, &CSItem::textAlignChanged, this, func);
    connect(f, &CSItem::bgColorChanged, this, func);
    connect(f, &CSItem::valuesChanged, this, func);
    connect(f, &CSItem::pageChanged, this, func);
    connect(f, &CSItem::readOnlyChanged, this, func);
    connect(f, &CSItem::formulaChanged, this, func);
    connect(f, &CSItem::idChanged, this, func);
    connect(f, &CSItem::labelChanged, this, func);

    endInsertRows();
    emit modelChanged();
    emit fieldAdded(f);
}
void FieldModel::insertField(CSItem* field, TreeSheetItem* parent, int pos)
{
    beginInsertRows(QModelIndex(), pos, pos);
    if(parent == m_rootSection.get())
    {
        m_rootSection->insertChild(field, pos);
    }
    endInsertRows();
    emit modelChanged();
}
Qt::ItemFlags FieldModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
        return Qt::ItemIsEnabled;

    // TreeSheetItem* childItem = static_cast<TreeSheetItem*>(index.internalPointer());
    if(m_colunm[index.column()]->getPos() == TreeSheetItem::ID)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    else if(m_colunm[index.column()]->getPos() == TreeSheetItem::TYPE)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
    }
    else if(m_colunm[index.column()]->getPos() == TreeSheetItem::FONT)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    else // if(!childItem->mayHaveChildren())
    {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable /*| Qt::ItemIsUserCheckable */;
    }
    /*else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable ;*/
}

QString FieldModel::getValue(const QString& key)
{
    return key;
}

QList<TreeSheetItem*> FieldModel::children()
{
    QList<TreeSheetItem*> result;
    for(int i= 0; i < m_rootSection->childrenCount(); ++i)
    {
        result.append(m_rootSection->childAt(i));
    }
    return result;
}

QList<CSItem*> FieldModel::allChildren() const
{
    return m_rootSection->allChildren();
}

QRectF FieldModel::childrenRect() const
{
    QRectF res{0., 0., 1., 1.};
    auto alls= allChildren();
    for(auto field : alls)
    {
        if(!field)
            continue;
        res= res.united(QRectF{field->x(), field->y(), field->width(), field->height()});
    }
    return res;
}

void FieldModel::getFieldFromPage(int pagePos, QList<CSItem*>& list)
{
    list= m_rootSection->fieldFromPage(pagePos);
}

FieldController* FieldModel::getFieldFromIndex(const QModelIndex& index)
{
    return static_cast<FieldController*>(index.internalPointer());
}

void FieldModel::updateItem(CSItem* item)
{
    int ind= m_rootSection->indexOfChild(item);
    if(ind >= 0)
    {
        emit dataChanged(createIndex(ind, 0, item), createIndex(ind, m_colunm.size(), item));
        emit modelChanged();
    }
    else
    {
        TreeSheetItem* parent= item->parentTreeItem();
        QList<TreeSheetItem*> list;
        while(parent != nullptr)
        {
            list.prepend(parent);
            parent= parent->parentTreeItem();
        }

        QModelIndex first;
        QModelIndex second;
        int i= 0;
        for(TreeSheetItem* itemtmp : list)
        {
            TreeSheetItem* next= nullptr;
            if(i + 1 > list.size())
            {
                next= list[++i];
            }
            else
            {
                next= item;
            }

            if(itemtmp == m_rootSection.get())
            {
                first= index(itemtmp->indexOfChild(next), 0, first);
                second= index(itemtmp->indexOfChild(next), m_colunm.size(), second);
            }
        }
        emit dataChanged(first, second);
        emit modelChanged();
    }
}

Section* FieldModel::getRootSection() const
{
    return m_rootSection.get();
}

void FieldModel::setRootSection(Section* rootSection)
{
    beginResetModel();
    m_rootSection.reset(rootSection);
    endResetModel();
}
void FieldModel::save(QJsonObject& json, bool exp)
{
    m_rootSection->save(json, exp);
}

void FieldModel::load(const QJsonObject& json)
{
    beginResetModel();
    m_rootSection->load(json);
    endResetModel();
}
void FieldModel::removeItem(QModelIndex& index)
{
    if(index.isValid())
    {
        TreeSheetItem* childItem= static_cast<TreeSheetItem*>(index.internalPointer());
        Section* parentSection= nullptr;
        if(index.parent().isValid())
        {
            TreeSheetItem* parentItem= static_cast<TreeSheetItem*>(index.internalPointer());
            parentSection= dynamic_cast<Section*>(parentItem);
        }
        else
        {
            parentSection= m_rootSection.get();
        }

        if(nullptr == parentSection)
        {
            return;
        }
        beginRemoveRows(index.parent(), parentSection->indexOfChild(childItem), parentSection->indexOfChild(childItem));

        parentSection->deleteChild(childItem);

        endRemoveRows();

        emit modelChanged();
    }
}

void FieldModel::removeField(FieldController* field)
{
    // int index = m_rootSection->indexOfChild(field);
    QList<TreeSheetItem*> ancestors;

    // ancestors.append(field);
    TreeSheetItem* tmp= field;
    while(tmp != nullptr)
    {
        tmp= tmp->parentTreeItem();
        if(nullptr != tmp)
        {
            ancestors.prepend(tmp);
        }
    }

    QModelIndex parent;
    TreeSheetItem* parentSection= nullptr;
    for(const auto& ancestor : ancestors)
    {
        if(nullptr != parentSection)
        {
            parent= index(parentSection->indexOfChild(ancestor), 0, parent);
            // TODO check that
            // parent= parent.child(parentSection->indexOfChild(ancestor), 0);
        }
        parentSection= ancestor;
    }

    beginRemoveRows(parent, parentSection->indexOfChild(field), parentSection->indexOfChild(field));

    parentSection->removeChild(field);

    endRemoveRows();
    emit modelChanged();
}
void FieldModel::clearModel()
{
    beginResetModel();
    m_rootSection->removeAll();
    endResetModel();
}

void FieldModel::setValueForAll(QModelIndex& index)
{
    if(index.isValid())
    {
        TreeSheetItem* childItem= static_cast<TreeSheetItem*>(index.internalPointer());
        m_rootSection->setValueForAll(childItem, m_colunm[index.column()]->getPos());
    }
}

void FieldModel::resetAllId()
{
    beginResetModel();
    int i= 0;
    m_rootSection->resetAllId(i);
    FieldController::setCount(i);
    endResetModel();
}
