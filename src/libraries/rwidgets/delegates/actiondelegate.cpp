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
#include "actiondelegate.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QToolButton>

#include "model/actiononlistmodel.h"

LabelWithOptions::LabelWithOptions(QWidget* parent)
{
    m_layout= new QHBoxLayout();
    setLayout(m_layout);

    m_label.reset(new QLabel());
    m_layout->addWidget(m_label.get());
}

void LabelWithOptions::setData(const QStringList icon, const QStringList name)
{
    if(icon.size() != name.size())
        return;

    while(m_layout->count() > 1)
    {
        m_layout->removeItem(m_layout->itemAt(1));
    }

    int i= 0;
    for(const auto& iconPath : icon)
    {
        auto btn= new QToolButton();
        btn->setIcon(QIcon::fromTheme(iconPath));
        btn->setToolTip(name[i]);
        m_layout->addWidget(btn);
        ++i;
    }
}

QString LabelWithOptions::text() const
{
    return m_label->text();
}

void LabelWithOptions::setText(const QString& text)
{
    if(this->text() == text)
        return;
    m_label->setText(text);
    emit textChanged();
}

// end of label

ActionDelegate::ActionDelegate(QObject* object) : m_label(new LabelWithOptions)
{
    // m_label->setVisible(true);
}

QWidget* ActionDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const
{

    auto listIcon= index.data(ActionOnListModel::PossibleIcon).toStringList();
    auto listname= index.data(ActionOnListModel::PossibleAction).toStringList();

    auto wid= new LabelWithOptions(parent);
    wid->setData(listIcon, listname);

    return wid;
}

QSize ActionDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize a= QStyledItemDelegate::sizeHint(option, index);
    a.setHeight(30);
    a.setWidth(150);
    return a;
}

void ActionDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);

    painter->save();
    qDebug() << "Action dElegate: Paint";
    auto listIcon= index.data(ActionOnListModel::PossibleIcon).toStringList();
    auto listname= index.data(ActionOnListModel::PossibleAction).toStringList();

    if(option.state & QStyle::State_Selected)
    {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    /*m_label->setData(listIcon, listname);
    m_label->setText(index.data().toString());
    m_label->resize(option.rect.size());
    qDebug() << option.rect.size() << index.data().toString();

    m_label->render(painter, QPoint(), QRegion(), QWidget::DrawChildren);*/

    auto textRect= option.rect;
    textRect.setRight(textRect.width() - (listIcon.size() * 32));

    painter->drawText(textRect, index.data().toString());
    int i= 1;
    for(const auto& icon : listIcon)
    {
        auto pic= QIcon::fromTheme(icon);

        painter->drawPixmap(option.rect.width() - (32 * i), option.rect.top(), pic.pixmap(QSize(25, 25)));
        ++i;
    }
    painter->restore();
}
