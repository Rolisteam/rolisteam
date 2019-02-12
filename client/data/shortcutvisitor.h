/***************************************************************************
 *   Copyright (C) 2017 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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
#ifndef SHORTCUTVISITOR_H
#define SHORTCUTVISITOR_H
#include <QAbstractItemModel>
#include <QObject>
class ShortCutModel;

class ShortcutVisitor : public QObject
{
    Q_OBJECT

public:
    ShortcutVisitor(QObject* parent= nullptr);
    ~ShortcutVisitor();

    QAbstractItemModel* getModel() const;
    bool registerWidget(QWidget* widget, const QString& categoryName, bool recursion= false);
    bool unregisterWidget(QWidget* widget);

private slots:
    void objectDeleted(QObject* obj);

private:
    void visit(QWidget* widget, const QString& category, bool recursion);

private:
    ShortCutModel* m_model;
    QMap<QWidget*, QString> m_categoriesNames;
};

#endif // SHORTCUTVISITOR_H
