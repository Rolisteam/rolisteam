/***************************************************************************
 *   Copyright (C) 2017 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#include "shortcutvisitor.h"
#include "data/shortcutmodel.h"

#include <QAction>
#include <QList>
#include <QWidget>

///////////////////////// CPP
ShortcutVisitor::ShortcutVisitor(QObject* parent) : QObject(parent), m_model(new ShortCutModel()) {}

ShortcutVisitor::~ShortcutVisitor() {}

QAbstractItemModel* ShortcutVisitor::getModel() const
{
    return m_model;
}

bool ShortcutVisitor::registerWidget(QWidget* widget, const QString& categoryName, bool recursion)
{
    if(m_categoriesNames.contains(widget))
        return false; // widget already registered

    m_categoriesNames.append(widget, categoryName);

    visit(widget, categoryName, recursion);
    connect(widget, &QWidget::destroyed, this, &ShortcutVisitor::objectDeleted);
    return true;
}

bool ShortcutVisitor::unregisterWidget(QWidget* widget)
{
    if(!m_categoriesNames.contains(widget))
        return false;

    const QString& category= m_categoriesNames[widget];

    if(category.isEmpty())
        return false;

    disconnect(widget, &QWidget::destroyed, this, &ShortcutVisitor::objectDeleted);
    m_model->removeCategory(category, true);
    return true;
}

void ShortcutVisitor::objectDeleted(QObject* obj)
{
    QWidget* widget= qobject_cast<QWidget*>(obj);
    if(nullptr != widget)
    {
        unregisterWidget(widget);
    }
}

void ShortcutVisitor::visit(QWidget* widget, const QString& category, bool recursion)
{
    QList<QAction*> actions= widget->actions();
    if(!actions.isEmpty())
    {
        m_model->addCategory(category);
    }
    for(QAction* action : actions)
    {
        // if(!action->shortcut().isEmpty())
        {
            QString actionName= action->text();
            actionName.remove('&');
            m_model->insertShortCut(category, actionName, action->shortcut().toString(QKeySequence::NativeText));
        }
    }

    if(recursion)
    {
        QObjectList children= widget->children();
        for(QObject* obj : children)
        {
            QWidget* wd= qobject_cast<QWidget*>(obj);
            if(nullptr != wd)
            {
                visit(wd, wd->objectName(), recursion);
            }
        }
    }
}
