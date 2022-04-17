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
#ifndef RWIDGETS_ACTIONDELEGATE_H
#define RWIDGETS_ACTIONDELEGATE_H

#include "rwidgets_global.h"
#include <QLabel>
#include <QStyledItemDelegate>
#include <memory>

class QHBoxLayout;
class RWIDGET_EXPORT LabelWithOptions : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
    LabelWithOptions(QWidget* parent= nullptr);
    ~LabelWithOptions();
    void setData(const QStringList icon, const QStringList name);

    QString text() const;
public slots:
    void setText(const QString& text);

signals:
    void optionSelected(int i);
    void textChanged();

private:
    std::unique_ptr<QLabel> m_label;
    QHBoxLayout* m_layout= nullptr;
};

class RWIDGET_EXPORT ActionDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ActionDelegate(QObject* parent= nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    std::unique_ptr<LabelWithOptions> m_label;
};

#endif // RWIDGETS_ACTIONDELEGATE_H
