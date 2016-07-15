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
#include "charactersheetbutton.h"

#include <QDebug>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

/////////////////////////////
/// CharacterSheetModel
/////////////////////////////

CharacterSheetModel::CharacterSheetModel()
 : m_characterCount(0),m_formulaManager(NULL)
{
    m_characterList = new QList<CharacterSheet*>;
    m_rootSection = new Section();
    m_formulaManager = new Formula::FormulaManager();

}

int CharacterSheetModel::rowCount ( const QModelIndex & parent  ) const
{
    if (parent.isValid())
    {
        CharacterSheetItem* tmp = static_cast<CharacterSheetItem*>(parent.internalPointer());
        return tmp->getChildrenCount();
    }
    else
    {
        return m_rootSection->getChildrenCount();
    }
}
CharacterSheet*  CharacterSheetModel::getCharacterSheet(int id)
{
    if((!m_characterList->isEmpty())&&(m_characterList->size()>id)&&(0<=id))
    {
        return m_characterList->at(id);
    }
    return NULL;
}

/*QList<CharacterSheetItem *>* CharacterSheetModel::getExportedList(CharacterSheet* character)
{
    QList<CharacterSheetItem *>* result = new QList<CharacterSheetItem *>();
    m_rootSection->fillList(result,character);
    return result;
}*/

int CharacterSheetModel::columnCount ( const QModelIndex & parent  ) const
{
    Q_UNUSED(parent)
    return m_characterList->size()+1;
}
QModelIndex CharacterSheetModel::index ( int row, int column, const QModelIndex & parent ) const
{
    
    if(row<0)
        return QModelIndex();
    
    CharacterSheetItem* parentItem = NULL;
    
    if (!parent.isValid())
        parentItem = m_rootSection;
    else
        parentItem = static_cast<CharacterSheetItem*>(parent.internalPointer());
    
    CharacterSheetItem* childItem = parentItem->getChildAt(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
    
    
}
QModelIndex CharacterSheetModel::parent ( const QModelIndex & index ) const
{
    
    if (!index.isValid())
        return QModelIndex();
    
    CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
    CharacterSheetItem* parentItem = childItem->getParent();
    
    if (parentItem == m_rootSection)
        return QModelIndex();
    
    return createIndex(parentItem->rowInParent(), 0, parentItem);
}

QVariant CharacterSheetModel::data ( const QModelIndex & index, int role  ) const
{
    if(!index.isValid())
        return QVariant();
    
    if((role == Qt::TextAlignmentRole) && (index.column()!=0))
        return Qt::AlignHCenter;
    if((role == Qt::DisplayRole)||(role == Qt::EditRole))
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(index.internalPointer());
        if(NULL!=childItem)
        {
            if(index.column()==0)
            {
                return childItem->getLabel();
            }
            else
            {
                QString path = childItem->getId();
                CharacterSheet* sheet = m_characterList->at(index.column()-1);
                return sheet->getValue(path,(Qt::ItemDataRole)role);
                //childItem->setValue(value.toString(),index.column()-1);
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
        if(NULL!=childItem)
        {
            if(index.column()==0)
            {
                childItem->setValue(value.toString());
            }
            else
            {
                QString path = childItem->getId();
                CharacterSheet* sheet = m_characterList->at(index.column()-1);
                QString valueStr = value.toString();
                QString formula;
                if(valueStr.startsWith('='))
                {
                    formula=valueStr;
                    QHash<QString,QString> hash = sheet->getVariableDictionnary();
                    m_formulaManager->setConstantHash(&hash);
                    valueStr=m_formulaManager->getValue(formula).toString();
                }
                sheet->setValue(path,valueStr,formula);
            }
            return true;
        }
    }
    return false;
}
CharacterSheet* CharacterSheetModel::addCharacterSheet()
{
    beginInsertColumns(QModelIndex(),m_characterList->size()+1 ,m_characterList->size()+1 );
    ++m_characterCount;
    CharacterSheet* sheet = new CharacterSheet;
    m_characterList->append(sheet);
    sheet->buildDataFromSection(m_rootSection);
    endInsertColumns();
    return sheet;
}
void CharacterSheetModel::addCharacterSheet(CharacterSheet* sheet)
{
    beginInsertColumns(QModelIndex(),m_characterList->size()+1 ,m_characterList->size()+1 );
    ++m_characterCount;
    m_characterList->append(sheet);
    emit characterSheetHasBeenAdded(sheet);
    endInsertColumns();
}

CharacterSheet *CharacterSheetModel::getCharacterSheetById(QString id)
{

    for(CharacterSheet* sheet :*m_characterList)
    {
        qDebug() << "sheet id"<< sheet->getUuid() << id;
    }
    for(CharacterSheet* sheet :*m_characterList)
    {
        if(sheet->getUuid() == id)
        {
            return sheet;
        }
    }
    return NULL;
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
    m_rootSection = rootSection;
}


QVariant CharacterSheetModel::headerData(int section, Qt::Orientation orientation,
                                         int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return tr("Fields name");
        default:
            return QString();
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
    sec->setName(title);
    rootSection->appendChild(sec);
    endInsertRows();
    return sec;
}

void CharacterSheetModel::addLine(const QModelIndex& index)
{
    QModelIndex parent = index;
    CharacterSheetItem* parentItem = NULL;
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
        field->setId(name);
        section->appendChild(field);
        endInsertRows();
    }
}
bool CharacterSheetModel::hasChildren ( const QModelIndex & parent  ) const
{
    
    if(!parent.isValid())//root
        return  m_rootSection->getChildrenCount()>0?true:false;
    else
    {
        CharacterSheetItem* childItem = static_cast<CharacterSheetItem*>(parent.internalPointer());
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
        return NULL;
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
    jsonObj["characterCount"]=m_characterCount;

    QJsonArray characters;
    foreach (CharacterSheet* item, *m_characterList)
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
    m_characterCount = jsonObj["characterCount"].toInt();
    QJsonArray characters = jsonObj["characters"].toArray();
    foreach(auto charJson, characters)
    {
        QJsonObject obj = charJson.toObject();
        CharacterSheet* sheet = new CharacterSheet();
        sheet->load(obj);
        m_characterList->append(sheet);
        emit characterSheetHasBeenAdded(sheet);
    }
    endResetModel();
}
