/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
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
#include "controller/view_controller/sidemenucontroller.h"

#include "mindmap/model/minditemmodel.h"

namespace mindmap
{
FilteredModel::FilteredModel()
{
    setDynamicSortFilter(true);
}

void FilteredModel::setPattern(const QString& newPattern)
{
    if(m_pattern == newPattern)
        return;
    m_pattern= newPattern;
    emit patternChanged();
    invalidateFilter();
}

bool FilteredModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    auto index= sourceModel()->index(source_row, 0, source_parent);

    auto type= index.data(MindItemModel::Type).value<MindItem::Type>();

    if(type != MindItem::NodeType)
        return false;

    if(m_pattern.isEmpty())
        return true;

    auto node= index.data(MindItemModel::Object).value<MindNode*>();
    if(!node)
        return false;
    auto parent= node->parentNode();

    bool res= false;
    switch(m_criteria)
    {
    case FilteredModel::NoCrit:
        res = true;
        break;
    case FilteredModel::NameCrit:
        res= node->text().contains(m_pattern, Qt::CaseInsensitive);
        break;
    case FilteredModel::DescriptionCrit:
        res= node->description().contains(m_pattern, Qt::CaseInsensitive);
        break;
    case FilteredModel::TagCrit:
        res= node->tagsText().contains(m_pattern, Qt::CaseInsensitive);
        break;
    case FilteredModel::ParentOfCrit:
        res= parent ? parent->id().contains(m_pattern, Qt::CaseInsensitive) : false;
        break;
    case FilteredModel::AllCrit:
        res= node->text().contains(m_pattern, Qt::CaseInsensitive) || node->tagsText().contains(m_pattern, Qt::CaseInsensitive) || node->description().contains(m_pattern, Qt::CaseInsensitive);
        break;
    }

    return res;
}

FilteredModel::Criteria FilteredModel::criteria() const
{
    return m_criteria;
}

void FilteredModel::setCriteria(Criteria newCriteria)
{
    if(m_criteria == newCriteria)
        return;
    m_criteria= newCriteria;
    emit criteriaChanged();
    invalidateFilter();
}

const QString& FilteredModel::pattern() const
{
    return m_pattern;
}

SideMenuController::SideMenuController(QObject* parent) : QObject{parent}, m_model{new mindmap::FilteredModel}
{
    connect(m_model.get(), &mindmap::FilteredModel::criteriaChanged, this, &SideMenuController::criteriaChanged);
    connect(m_model.get(), &mindmap::FilteredModel::patternChanged, this, &SideMenuController::patternChanged);
}

mindmap::MindMapControllerBase* SideMenuController::controller() const
{
    return m_controller;
}

void SideMenuController::setController(mindmap::MindMapControllerBase* newController)
{
    if(m_controller == newController)
        return;
    m_controller= newController;
    emit controllerChanged();
    m_model->setSourceModel(newController->itemModel());
    connect(m_controller, &mindmap::MindMapControllerBase::nameChanged, this, &SideMenuController::nameChanged);
    emit nameChanged();
}

FilteredModel* SideMenuController::model() const
{
    return m_model.get();
}

const QString& SideMenuController::pattern() const
{
    return m_model->pattern();
}

void SideMenuController::setPattern(const QString& newPattern)
{
    m_model->setPattern(newPattern);
}

FilteredModel::Criteria SideMenuController::criteria() const
{
    return m_model->criteria();
}

void SideMenuController::setCriteria(FilteredModel::Criteria newCriteria)
{
    m_model->setCriteria(newCriteria);
}

QString SideMenuController::name() const
{
    return m_controller ? m_controller->name() : QString();
}

void SideMenuController::setName(const QString &newName)
{
    if(m_controller)
        m_controller->setName(newName);
}

} // namespace mindmap
