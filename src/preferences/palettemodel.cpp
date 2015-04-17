/***************************************************************************
*	Copyright (C) 2015 by Renaud Guezennec                                *
*   http://renaudguezennec.homelinux.org/accueil,3.html                   *
*                                                                         *
*   Rolisteam is free software; you can redistribute it and/or modify     *
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
#include "palettemodel.h"



PaletteColor::PaletteColor(QColor color,QString str,QPalette::ColorGroup group,QPalette::ColorRole role)
    : m_color(color),m_name(str),m_group(group),m_role(role)
{

}

void PaletteColor::setColor(QColor a)
{
    m_color = a;
}

void PaletteColor::setName(QString a)
{
    m_name = a;
}
QColor PaletteColor::getColor()
{
    return m_color;
}

QString PaletteColor::getName()
{
    return m_name;
}
QPalette::ColorGroup PaletteColor::getGroup()
{
    return m_group;
}
QPalette::ColorRole PaletteColor::getRole()
{
    return m_role;
}

/////////////////////////////////
// PaletteModel
/////////////////////////////////
PaletteModel::PaletteModel()
{
    m_header << tr("Role") << tr("Group");
    initData();
}

PaletteModel::~PaletteModel()
{

}

void PaletteModel::initData()
{
    PaletteColor* color = NULL;

    color = new PaletteColor(QColor(),"WindowText",QPalette::Active,QPalette::WindowText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Button",QPalette::Active,QPalette::Button);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Light",QPalette::Active,QPalette::Light);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Midlight",QPalette::Active,QPalette::Midlight);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Dark",QPalette::Active,QPalette::Dark);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Mid",QPalette::Active,QPalette::Mid);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Text",QPalette::Active,QPalette::Text);
    m_data.append(color);
    color = new PaletteColor(QColor(),"BrightText",QPalette::Active,QPalette::BrightText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Base",QPalette::Active,QPalette::Base);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Window",QPalette::Active,QPalette::Window);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Shadow",QPalette::Active,QPalette::Shadow);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Highlight",QPalette::Active,QPalette::Highlight);
    m_data.append(color);
    color = new PaletteColor(QColor(),"WindowText",QPalette::Active,QPalette::HighlightedText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Link",QPalette::Active,QPalette::Link);
    m_data.append(color);
    color = new PaletteColor(QColor(),"LinkVisited",QPalette::Active,QPalette::LinkVisited);
    m_data.append(color);
    color = new PaletteColor(QColor(),"AlternateBase",QPalette::Active,QPalette::AlternateBase);
    m_data.append(color);
    color = new PaletteColor(QColor(),"ToolTipBase",QPalette::Active,QPalette::ToolTipBase);
    m_data.append(color);
    color = new PaletteColor(QColor(),"ToolTipText",QPalette::Active,QPalette::ToolTipText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"NoRole",QPalette::Active,QPalette::NoRole);
    m_data.append(color);

    color = new PaletteColor(QColor(),"WindowText",QPalette::Inactive,QPalette::WindowText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Button",QPalette::Inactive,QPalette::Button);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Light",QPalette::Inactive,QPalette::Light);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Midlight",QPalette::Inactive,QPalette::Midlight);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Dark",QPalette::Inactive,QPalette::Dark);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Mid",QPalette::Inactive,QPalette::Mid);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Text",QPalette::Inactive,QPalette::Text);
    m_data.append(color);
    color = new PaletteColor(QColor(),"BrightText",QPalette::Inactive,QPalette::BrightText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Base",QPalette::Inactive,QPalette::Base);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Window",QPalette::Inactive,QPalette::Window);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Shadow",QPalette::Inactive,QPalette::Shadow);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Highlight",QPalette::Inactive,QPalette::Highlight);
    m_data.append(color);
    color = new PaletteColor(QColor(),"WindowText",QPalette::Inactive,QPalette::HighlightedText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Link",QPalette::Inactive,QPalette::Link);
    m_data.append(color);
    color = new PaletteColor(QColor(),"LinkVisited",QPalette::Inactive,QPalette::LinkVisited);
    m_data.append(color);
    color = new PaletteColor(QColor(),"AlternateBase",QPalette::Inactive,QPalette::AlternateBase);
    m_data.append(color);
    color = new PaletteColor(QColor(),"ToolTipBase",QPalette::Inactive,QPalette::ToolTipBase);
    m_data.append(color);
    color = new PaletteColor(QColor(),"ToolTipText",QPalette::Inactive,QPalette::ToolTipText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"NoRole",QPalette::Inactive,QPalette::NoRole);
    m_data.append(color);

    color = new PaletteColor(QColor(),"WindowText",QPalette::Disabled,QPalette::WindowText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Button",QPalette::Disabled,QPalette::Button);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Light",QPalette::Disabled,QPalette::Light);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Midlight",QPalette::Disabled,QPalette::Midlight);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Dark",QPalette::Disabled,QPalette::Dark);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Mid",QPalette::Disabled,QPalette::Mid);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Text",QPalette::Disabled,QPalette::Text);
    m_data.append(color);
    color = new PaletteColor(QColor(),"BrightText",QPalette::Disabled,QPalette::BrightText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Base",QPalette::Disabled,QPalette::Base);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Window",QPalette::Disabled,QPalette::Window);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Shadow",QPalette::Disabled,QPalette::Shadow);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Highlight",QPalette::Disabled,QPalette::Highlight);
    m_data.append(color);
    color = new PaletteColor(QColor(),"WindowText",QPalette::Disabled,QPalette::HighlightedText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"Link",QPalette::Disabled,QPalette::Link);
    m_data.append(color);
    color = new PaletteColor(QColor(),"LinkVisited",QPalette::Disabled,QPalette::LinkVisited);
    m_data.append(color);
    color = new PaletteColor(QColor(),"AlternateBase",QPalette::Disabled,QPalette::AlternateBase);
    m_data.append(color);
    color = new PaletteColor(QColor(),"ToolTipBase",QPalette::Disabled,QPalette::ToolTipBase);
    m_data.append(color);
    color = new PaletteColor(QColor(),"ToolTipText",QPalette::Disabled,QPalette::ToolTipText);
    m_data.append(color);
    color = new PaletteColor(QColor(),"NoRole",QPalette::Disabled,QPalette::NoRole);
    m_data.append(color);
}
int PaletteModel::rowCount(const QModelIndex &parent) const
{
    return m_data.size();
}
int  PaletteModel::columnCount(const QModelIndex &parent) const
{
    return m_header.size();
}
QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    PaletteColor* color = m_data.at(index.row());

    if(Qt::DecorationRole == role)
    {
        if(index.column()==0)
        {
         return color->getColor();
        }
    }
    else if(Qt::DisplayRole == role)
    {
        if(index.column()==0)
        {
            return color->getName();
        }
        else
        {
            return (int)color->getGroup();
        }
    }
    return QVariant();
}
void PaletteModel::setPalette(QPalette palette)
{
    //dataChanged(QModelIndex());
    beginResetModel();
    foreach(PaletteColor* color, m_data)
    {
       color->setColor(palette.color(color->getGroup(),color->getRole()));
    }
    endResetModel();
}
