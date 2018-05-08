/***************************************************************************
    *   Copyright (C) 2016 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                                      *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
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
#include "pagedelegate.h"
#include <QComboBox>
#include "charactersheetitem.h"

int PageDelegate::m_pageNumber = 1;
const QString pageName(PageDelegate::tr("Page %1"));
PageDelegate::PageDelegate(QWidget* parent)
    : QStyledItemDelegate(parent)
{

}
QString PageDelegate::displayText(const QVariant &value, const QLocale &locale) const
{
    if(value.toInt() < 0)
    {
        return tr("All pages");
    }
    else
    {
        return pageName.arg(value.toInt()+1);
    }
}
QWidget* PageDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // ComboBox ony in column 2
       if (index.column() != static_cast<int>(CharacterSheetItem::PAGE))
           return QStyledItemDelegate::createEditor(parent, option, index);


    QComboBox* cm = new QComboBox(parent);
    cm->setEditable(true);
    cm->insertItem(0,tr("All Pages"));
    for(int i=0;i<m_pageNumber;++i)
    {
        cm->insertItem(i+1,pageName.arg(i+1));
    }
    return cm;
}
void PageDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor))
    {
       int currentIndex = index.data(Qt::EditRole).toInt();
       if (currentIndex >= 0)
       {
           cb->setCurrentIndex(currentIndex+1);
       }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}
void PageDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    if (QComboBox* cb = qobject_cast<QComboBox*>(editor))
    {
        model->setData(index, cb->currentIndex()-1, Qt::EditRole);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}
void PageDelegate::setPageNumber(int page)
{
    m_pageNumber = page;
}
