/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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

#include "charactersheetmodel.h"
#include "charactersheet.h"
#include "section.h"
#include "field.h"
#include "tablefield.h"
//#include "charactersheetbutton.h"

#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

/////////////////////////////
/// CharacterSheetModel
/////////////////////////////

CharacterSheetModel::CharacterSheetModel()
 : m_formulaManager(nullptr) //m_characterCount(0),
{
    m_characterList = new QList<CharacterSheet*>;
    m_rootSection = new Section();
    m_formulaManager = new Formula::FormulaManager();

}
CharacterSheetModel::~CharacterSheetModel()
{
    qDeleteAll(*m_characterList);
    delete m_characterList;
    delete m_rootSection;
    delete m_formulaManager;
}

int CharacterSheetModel::rowCount ( const QModelIndex & parent  ) const
{
    int val = 0;
    if(parent.isValid())
    {
        CharacterSheetItem* tmp = static_cast<CharacterSheetItem*>(parent.internalPointer());
        if(tmp->getFieldType() == Field::TABLE)
        {
            int max = tmp->getChildrenCount();
            auto result = std::max_element(m_characterList->begin(),m_characterList->end(), [tmp](CharacterSheet* a, CharacterSheet* b){
                auto fieldA = a->getFieldFromKey(tmp->getId());
                auto fieldB = b->getFieldFromKey(tmp->getId());
                return fieldA->getChildrenCount() < fieldB->getChildrenCount();
            });
            auto maxfield = (*result)->getFieldFromKey(tmp->getId());
            val = std::max(max, maxfield->getChildrenCount());
        }
        else if(tmp)
            val = tmp->getChildrenCount();
        else
            qDebug() << "parent is valid but internal pointer is not";
    }
    else
    {
        val = m_rootSection->getChildrenCount();
    }
    return val;
}
CharacterSheet*  CharacterSheetModel::getCharacterSheet(int id)
{
    if((!m_characterList->isEmpty())&&(m_characterList->size()>id)&&(0<=id))
    {
        return m_characterList->at(id);
    }
    return nullptr;
}

int CharacterSheetModel::columnCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent)
    return m_characterList->size()+1;
}
QModelIndex CharacterSheetModel::index ( int row, int column, const QModelIndex & parent ) const
{
    if(row<0)
        return QModelIndex();
    
    CharacterSheetItem* parentItem = nullptr;
    CharacterSheetItem* childItem = nullptr;
    CharacterSheet* sheet = nullptr;
    
    if (!parent.isValid() && column == 0)
    {
        parentItem = m_rootSection;
    }
    else if(parent.isValid())
    {
        parentItem = static_cast<CharacterSheetItem*>(parent.internalPointer());
    }
    else if(!parent.isValid() && column != 0)
    {
        sheet = m_characterList->at(column-1);
    }

    if(nullptr != parentItem)
    {
        childItem = parentItem->getChildAt(row);
    }
    else
    {
        childItem = sheet->getFieldAt(row);
    }
    
    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}
QModelIndex CharacterSheetModel::parent ( const QModelIndex & index ) const
{
    if (!index.isValid())
        return QModelIndex();
    
    CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());

    if(nullptr == childItem)
        return QModelIndex();

    CharacterSheetItem* parentItem = childItem->getParent();
    
    if (parentItem == m_rootSection || parentItem == nullptr)
    {
        return QModelIndex();
    }
    
    return createIndex(parentItem->rowInParent(), 0, parentItem);
}

QVariant CharacterSheetModel::data ( const QModelIndex & index, int role  ) const
{
    if(!index.isValid())
        return QVariant();
    
    if((role == Qt::TextAlignmentRole) && (index.column()!=0))
        return Qt::AlignHCenter;
    if((role == Qt::DisplayRole)||(role == Qt::EditRole)||(role == Qt::BackgroundRole))
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
        if(nullptr!=childItem)
        {
            CharacterSheetItem* parentItem = childItem->getParent();
            if(role == Qt::BackgroundRole)
            {
                if(0!=index.column())
                {
                    QString path = childItem->getPath();
                    CharacterSheet* sheet = m_characterList->at(index.column()-1);
                    bool isReadOnly = sheet->getValue(path,Qt::BackgroundRole).toBool();
                    if(isReadOnly)
                    {
                        return QColor(128,128,128);
                    }
                }
            }
            else
            {
                if(index.column()==0)
                {
                    return childItem->getLabel();
                }
                else
                {
                    if(parentItem && parentItem->getFieldType() == Field::TABLE)
                    {
                        QString path = parentItem->getPath();
                        CharacterSheet* sheet = m_characterList->at(index.column()-1);
                        auto table = sheet->getFieldFromKey(path);
                        auto child = table->getChildAt(index.row());
                        if(child == nullptr )
                        {
                            return {};
                        }
                        if(role == Qt::DisplayRole)
                        {
                            return child->value();
                        }
                        else if(role == Qt::EditRole)
                        {
                            auto val = child->getFormula();
                            if(val.isEmpty())
                                val = child->value();
                            return val;
                        }
                    }
                    else
                    {
                        QString path = childItem->getPath();
                        CharacterSheet* sheet = m_characterList->at(index.column()-1);
                        return sheet->getValue(path,static_cast<Qt::ItemDataRole>(role));
                    }
                }
            }
        }
    }
    return QVariant();
}

bool CharacterSheetModel::setData ( const QModelIndex& index, const QVariant & value, int role  )
{
    if(Qt::EditRole==role)
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());

        if(nullptr!=childItem)
        {
            if(index.column()==0)
            {
                emit dataCharacterChange();
                childItem->setLabel(value.toString());
            }
            else
            {
                CharacterSheetItem* parentItem = childItem->getParent();
                QString formula;
                auto valueStr = value.toString();
                if(parentItem && parentItem->getFieldType() == Field::TABLE)
                {
                    QString path = parentItem->getPath();
                    CharacterSheet* sheet = m_characterList->at(index.column()-1);
                    auto table = sheet->getFieldFromKey(path);
                    auto child = table->getChildAt(index.row());
                    if(nullptr == child)
                        return false;
                    if(valueStr.startsWith('='))
                    {
                        formula=valueStr;
                        QHash<QString,QString> hash = sheet->getVariableDictionnary();
                        m_formulaManager->setConstantHash(&hash);
                        valueStr=m_formulaManager->getValue(formula).toString();
                        child->setFormula(formula);
                    }
                    child->setValue(valueStr);
                }
                else
                {
                    QString path = childItem->getPath();
                    CharacterSheet* sheet = m_characterList->at(index.column()-1);
                    if(valueStr.startsWith('='))
                    {
                        formula=valueStr;
                        QHash<QString,QString> hash = sheet->getVariableDictionnary();
                        m_formulaManager->setConstantHash(&hash);
                        valueStr=m_formulaManager->getValue(formula).toString();
                    }

                    CharacterSheetItem* newitem = sheet->setValue(path,valueStr,formula);
                    if(nullptr!=newitem)
                    {
                        newitem->setLabel(childItem->getLabel());
                        newitem->setOrig(childItem);
                    }
                    computeFormula(childItem->getLabel(),sheet);
                }
                emit dataCharacterChange();
            }
            return true;
        }
    }
    return false;
}
void CharacterSheetModel::computeFormula(QString path,CharacterSheet* sheet)
{
    QStringList List = sheet->getAllDependancy(path);

    for(auto& item : List)
    {
        QString formula;
        QString valueStr;

        QHash<QString,QString> hash = sheet->getVariableDictionnary();
        m_formulaManager->setConstantHash(&hash);
        formula = sheet->getValue(item,Qt::EditRole).toString();
        valueStr=m_formulaManager->getValue(formula).toString();
        sheet->setValue(item,valueStr,formula);
    }
}
void CharacterSheetModel::fieldHasBeenChanged(CharacterSheet* sheet,CharacterSheetItem* item, const QString&)
{
   emit dataCharacterChange();
   computeFormula(item->getLabel(),sheet);
}
CharacterSheet* CharacterSheetModel::addCharacterSheet()
{
    CharacterSheet* sheet = new CharacterSheet;
    addCharacterSheet(sheet,false);

    sheet->buildDataFromSection(m_rootSection);
    return sheet;
}

void CharacterSheetModel::clearModel()
{
    beginResetModel();
    qDeleteAll(*m_characterList);
    m_characterList->clear();
    if(nullptr != m_rootSection)
    {
        m_rootSection->removeAll();
    }
    endResetModel();
}

void CharacterSheetModel::checkCharacter(Section *section)
{
    for(auto& sheet : *m_characterList)
    {
        for(int i = 0; i <  section->getChildrenCount(); ++i)
        {
            auto id = section->getChildAt(i);
            auto field = sheet->getFieldFromKey(id->getId());
            if(nullptr == field && id->getFieldType() != CharacterSheetItem::TABLE)
            {
                Field* newField = new Field(false);
                newField->copyField(id,true);
                sheet->insertCharacterItem(newField);
                field = newField;
            }
            else if(nullptr == field && id->getFieldType() == CharacterSheetItem::TABLE)
            {
                TableField* newtablefield = new TableField(false);
                newtablefield->copyField(id,true);
                sheet->insertCharacterItem(newtablefield);
                field = newtablefield;
            }
            for(int j = 0; j < id->getChildrenCount(); ++j)
            {
                auto childFormat = id->getChildAt(j);
                auto childCharacter = field->getChildAt(j);
                if(nullptr != childFormat && nullptr != childCharacter)
                {
                    if(childFormat->getId() != childCharacter->getId())
                    {
                        childCharacter->setId(childFormat->getId());
                    }
                }
            }
            if(field->getFieldType() != id->getFieldType())
            {
                field->setCurrentType(id->getFieldType());
            }
            if(field->getLabel() != id->getLabel())
            {
                field->setLabel(id->getLabel());
            }
        }
    }
}
void CharacterSheetModel::addCharacterSheet(CharacterSheet* sheet, bool reset, int pos)
{
    if(pos<0)
    {
        pos = m_characterList->size()+1;
    }
    if(!reset)
        beginInsertColumns(QModelIndex(),pos,pos);
    m_characterList->insert(pos-1,sheet);
    emit characterSheetHasBeenAdded(sheet);
    if(!reset)
    {
        endInsertColumns();

        emit dataCharacterChange();
    }
    connect(sheet,&CharacterSheet::updateField,this,&CharacterSheetModel::fieldHasBeenChanged);
    connect(sheet,&CharacterSheet::addLineToTableField,this,&CharacterSheetModel::addSubChild);
}

void CharacterSheetModel::addSubChildRoot(CharacterSheetItem* item)
{
    if(!m_rootSection)
        return;

    auto parentItem = m_rootSection->getChildAt(item->getPath());
    auto r = m_rootSection->indexOfChild(parentItem);
    auto structTable = dynamic_cast<TableField*>(parentItem);
    if(structTable == nullptr)
        return;

    auto addedFieldCount = structTable->itemPerLine();
    auto index = createIndex(r,0,parentItem);
    beginInsertRows(index,parentItem->getChildrenCount(),parentItem->getChildrenCount()+addedFieldCount);
    structTable->appendChild(nullptr);
    endInsertRows();

}

void CharacterSheetModel::addSubChild(CharacterSheet* sheet, CharacterSheetItem* item)
{
    if(!m_rootSection)
        return;

    auto c = m_characterList->indexOf(sheet)+1;
    auto parentItem = m_rootSection->getChildAt(item->getPath());
    auto r = m_rootSection->indexOfChild(parentItem);
    auto table = dynamic_cast<TableField*>(item);
    auto structTable = dynamic_cast<TableField*>(parentItem);
    if(table == nullptr || structTable == nullptr)
        return;

    auto lineCount = table->lineNumber();
    auto structLineCount = structTable->lineNumber();
    auto addedFieldCount = table->itemPerLine();
    if(lineCount<0 || structLineCount<0 || addedFieldCount<0)
        return;

    if(lineCount == structLineCount)
    {
        auto index = createIndex(r,0,parentItem);
        beginInsertRows(index,parentItem->getChildrenCount(),parentItem->getChildrenCount()+addedFieldCount);
        structTable->appendChild(nullptr);
        endInsertRows();
    }

    auto index = createIndex(r,c,item);
    beginInsertRows(index,item->getChildrenCount(),item->getChildrenCount()+addedFieldCount);
    //generic method - for tableview the item is built from the last one.
    table->appendChild(nullptr);
    checkTableItem();
    endInsertRows();
}
void CharacterSheetModel::removeCharacterSheet(CharacterSheet* sheet)
{
    int pos = m_characterList->indexOf(sheet);

    if(pos>=0)
    {
        beginRemoveColumns(QModelIndex(),pos+1,pos+1);

        m_characterList->removeAt(pos);

        endRemoveColumns();
    }

}

void CharacterSheetModel::removeCharacterSheet(QModelIndex & index)
{
    beginRemoveColumns(QModelIndex(),index.column(),index.column());

    m_characterList->removeAt(index.column()-1);

    endRemoveColumns();
}

CharacterSheet *CharacterSheetModel::getCharacterSheetById(QString id)
{
    for(auto& sheet :*m_characterList)
    {
        if(sheet->getUuid() == id)
        {
            return sheet;
        }
    }
    return nullptr;
}

int CharacterSheetModel::getCharacterSheetCount() const
{
    if(nullptr!=m_characterList)
    {
        return m_characterList->size();
    }
    else
    {
        return 0;
    }
}
#ifndef RCSE
void CharacterSheetModel::readRootSection(NetworkMessageReader* msg)
{
    beginResetModel();
    QList<QGraphicsScene*> scenes;
    QByteArray array = msg->byteArray32();
    QJsonDocument doc = QJsonDocument::fromBinaryData(array);
    QJsonObject obj = doc.object();
    m_rootSection->load(obj,scenes);
    endResetModel();
}

void CharacterSheetModel::fillRootSection(NetworkMessageWriter* msg)
{
    //m_rootSection->fillNetworkMessage(msg);
    QJsonDocument doc;
    QJsonObject data;
    m_rootSection->save(data);
    doc.setObject(data);
    msg->byteArray32(doc.toBinaryData());
}
#endif
Section* CharacterSheetModel::getRootSection() const
{
    return m_rootSection;
}

void CharacterSheetModel::setRootSection(Section *rootSection)
{
    auto previous = m_rootSection;
    beginResetModel();
    m_rootSection = rootSection;
    endResetModel();
    if(m_rootSection != previous)
        connect(m_rootSection,&Section::addLineToTableField,this,&CharacterSheetModel::addSubChildRoot);
}


QVariant CharacterSheetModel::headerData(int section, Qt::Orientation orientation,
                                         int role) const
{
    if(role  == Qt::TextAlignmentRole)
        return Qt::AlignCenter;

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Fields name");
        default:
            {
                if(m_characterList->size() > (section-1))
                {
                    auto character = m_characterList->at(section-1);
                    return character->getName();
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
Qt::ItemFlags CharacterSheetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}
void CharacterSheetModel::addSection()
{
    addSection(tr("Empty Section %1").arg(m_rootSection->getChildrenCount()+1));
}
CharacterSheetItem* CharacterSheetModel::addSection(QString title)
{
    beginInsertRows(QModelIndex(),m_rootSection->getChildrenCount(),m_rootSection->getChildrenCount());
    Section* rootSection = static_cast<Section*>(m_rootSection);
    Section* sec=new Section();
    sec->setLabel(title);
    sec->setId(tr("Section_%1").arg(m_rootSection->getChildrenCount()+1));
    rootSection->appendChild(sec);
    endInsertRows();
    emit dataCharacterChange();
    return sec;
}

void CharacterSheetModel::addLine(const QModelIndex& index)
{
    QModelIndex parent = index;
    CharacterSheetItem* parentItem = nullptr;
    if(index.isValid())
    {
        parentItem = static_cast<CharacterSheetItem*>(index.internalPointer());
    }
    else
    {
        parentItem = m_rootSection;
    }
    if(!parentItem->mayHaveChildren())
    {
        parentItem = parentItem->getParent();
        parent = parent.parent();
    }
    addLine(parentItem,tr("Field %1").arg(parentItem->getChildrenCount()),parent);

}
void CharacterSheetModel::addLine(CharacterSheetItem* parentItem,QString name,const QModelIndex& parent)
{
    if(parentItem->mayHaveChildren())
    {
        beginInsertRows(parent,parentItem->getChildrenCount(),parentItem->getChildrenCount());
        Section* section = static_cast<Section*>(parentItem);
        Field* field = new Field();
        field->setId(name.replace(' ','_'));
        field->setLabel(name);
        section->appendChild(field);
        endInsertRows();
        emit dataCharacterChange();
    }
}
bool CharacterSheetModel::hasChildren ( const QModelIndex & parent  ) const
{
    
    if(!parent.isValid())//root
        return  m_rootSection->getChildrenCount()>0?true:false;
    else
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(parent.internalPointer());
        if(nullptr == childItem)
            return false;

        if(childItem->getChildrenCount()==0)
            return false;
        else
            return true;


    }
}
CharacterSheetItem* CharacterSheetModel::indexToSection(const QModelIndex & index)
{
    if(index.isValid())
        return static_cast<CharacterSheetItem*>(index.internalPointer());
    else
        return nullptr;
}
QModelIndex CharacterSheetModel::indexToSectionIndex(const QModelIndex & index)
{
    if(index.parent().isValid())//if parent is valid it's a field, return its parent (the section).
        return index.parent();
    else
        return index;
}
bool CharacterSheetModel::writeModel(QJsonObject& jsonObj, bool writeData)
{
    if(writeData)
    {
        QJsonObject data;
        m_rootSection->save(data);
        jsonObj["data"]=data;
    }
    jsonObj["characterCount"]=m_characterList->size();//m_characterCount;

    QJsonArray characters;
    for(auto& item : *m_characterList)
    {
        QJsonObject charObj;
        item->save(charObj);
        characters.append(charObj);
    }
    jsonObj["characters"]=characters;
    return true;
}

void CharacterSheetModel::readModel(QJsonObject& jsonObj,bool readRootSection)
{
    beginResetModel();
    if(readRootSection)
    {
        QJsonObject data = jsonObj["data"].toObject();
        m_rootSection->load(data,QList<QGraphicsScene*>());
    }
    QJsonArray characters = jsonObj["characters"].toArray();
    for(const auto charJson : characters)
    {
        QJsonObject obj = charJson.toObject();
        CharacterSheet* sheet = new CharacterSheet();
        sheet->load(obj);
        sheet->setOrigin(m_rootSection);
        addCharacterSheet(sheet,true);
    }
    checkTableItem();
    endResetModel();
}

void CharacterSheetModel::checkTableItem()
{
    for(int i = 0; i < m_rootSection->getChildrenCount(); ++i)
    {
        auto child = m_rootSection->getChildAt(i);
        if(CharacterSheetItem::TableItem == child->getItemType())
        {
            for(auto& character : *m_characterList)
            {
                auto childFromCharacter = character->getFieldAt(i);
                auto table = dynamic_cast<TableField*>(child);
                while(childFromCharacter->getChildrenCount() > child->getChildrenCount())
                {
                    table->appendChild(nullptr);
                }
            }
        }
    }


}

