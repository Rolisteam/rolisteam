/***************************************************************************
 *	Copyright (C) 2015 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
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
#include <QJsonArray>

PaletteColor::PaletteColor(QColor color, QString str, QPalette::ColorGroup group, QPalette::ColorRole role)
    : m_color(color), m_name(str), m_group(group), m_role(role)
{
}

void PaletteColor::setColor(QColor a)
{
    m_color= a;
}

void PaletteColor::setName(QString a)
{
    m_name= a;
}
QColor PaletteColor::getColor() const
{
    return m_color;
}

QString PaletteColor::getName() const
{
    return m_name;
}
QPalette::ColorGroup PaletteColor::getGroup() const
{
    return m_group;
}
QPalette::ColorRole PaletteColor::getRole() const
{
    return m_role;
}
void PaletteColor::writeTo(QJsonObject& json)
{
    json["role"]= static_cast<int>(m_role);
    json["group"]= static_cast<int>(m_group);
    json["name"]= m_name;
    json["color"]= m_color.name();
}
bool PaletteColor::readFrom(QJsonObject& json)
{
    m_role= static_cast<QPalette::ColorRole>(json["role"].toInt());
    m_group= static_cast<QPalette::ColorGroup>(json["group"].toInt());
    m_name= json["name"].toString();
    QString colorName= json["color"].toString();
    m_color.setNamedColor(colorName);
    return true;
}

/////////////////////////////////
// PaletteModel
/////////////////////////////////
PaletteModel::PaletteModel(QObject* parent) : QAbstractTableModel(parent)
{
    m_header << tr("Role") << tr("Group");
    m_groupList << tr("Active") << tr("Disable") << tr("Inactive");
    initData();
}

PaletteModel::~PaletteModel()
{
    qDeleteAll(m_data);
    m_data.clear();
}
QVariant PaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(Qt::DisplayRole == role)
    {
        if(orientation == Qt::Horizontal)
        {
            return m_header.at(section);
        }
    }
    return QVariant();
}
void PaletteModel::initData()
{
    PaletteColor* color= nullptr;

    // Active
    color= new PaletteColor(QColor(), "Window", QPalette::Active, QPalette::Window);
    m_data.append(color);
    color= new PaletteColor(QColor(), "WindowText", QPalette::Active, QPalette::WindowText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Base", QPalette::Active, QPalette::Base);
    m_data.append(color);
    color= new PaletteColor(QColor(), "AlternateBase", QPalette::Active, QPalette::AlternateBase);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ToolTipBase", QPalette::Active, QPalette::ToolTipBase);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ToolTipText", QPalette::Active, QPalette::ToolTipText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "PlaceholderText", QPalette::Active, QPalette::PlaceholderText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Text", QPalette::Active, QPalette::Text);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Button", QPalette::Active, QPalette::Button);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ButtonText", QPalette::Active, QPalette::ButtonText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "BrightText", QPalette::Active, QPalette::BrightText);
    m_data.append(color);

    color= new PaletteColor(QColor(), "Light", QPalette::Active, QPalette::Light);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Midlight", QPalette::Active, QPalette::Midlight);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Dark", QPalette::Active, QPalette::Dark);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Mid", QPalette::Active, QPalette::Mid);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Shadow", QPalette::Active, QPalette::Shadow);
    m_data.append(color);

    color= new PaletteColor(QColor(), "Highlight", QPalette::Active, QPalette::Highlight);
    m_data.append(color);
    color= new PaletteColor(QColor(), "HighlightedText", QPalette::Active, QPalette::HighlightedText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Link", QPalette::Active, QPalette::Link);
    m_data.append(color);
    color= new PaletteColor(QColor(), "LinkVisited", QPalette::Active, QPalette::LinkVisited);
    m_data.append(color);
    color= new PaletteColor(QColor(), "NoRole", QPalette::Active, QPalette::NoRole);
    m_data.append(color);

    // Inactive
    color= new PaletteColor(QColor(), "WindowText", QPalette::Inactive, QPalette::WindowText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Button", QPalette::Inactive, QPalette::Button);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ButtonText", QPalette::Inactive, QPalette::ButtonText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Light", QPalette::Inactive, QPalette::Light);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Midlight", QPalette::Inactive, QPalette::Midlight);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Dark", QPalette::Inactive, QPalette::Dark);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Mid", QPalette::Inactive, QPalette::Mid);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Text", QPalette::Inactive, QPalette::Text);
    m_data.append(color);
    color= new PaletteColor(QColor(), "BrightText", QPalette::Inactive, QPalette::BrightText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Base", QPalette::Inactive, QPalette::Base);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Window", QPalette::Inactive, QPalette::Window);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Shadow", QPalette::Inactive, QPalette::Shadow);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Highlight", QPalette::Inactive, QPalette::Highlight);
    m_data.append(color);
    color= new PaletteColor(QColor(), "HighlightedText", QPalette::Inactive, QPalette::HighlightedText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Link", QPalette::Inactive, QPalette::Link);
    m_data.append(color);
    color= new PaletteColor(QColor(), "LinkVisited", QPalette::Inactive, QPalette::LinkVisited);
    m_data.append(color);
    color= new PaletteColor(QColor(), "AlternateBase", QPalette::Inactive, QPalette::AlternateBase);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ToolTipBase", QPalette::Inactive, QPalette::ToolTipBase);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ToolTipText", QPalette::Inactive, QPalette::ToolTipText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "NoRole", QPalette::Inactive, QPalette::NoRole);
    m_data.append(color);
    color= new PaletteColor(QColor(), "PlaceholderText", QPalette::Inactive, QPalette::PlaceholderText);
    m_data.append(color);

    color= new PaletteColor(QColor(), "WindowText", QPalette::Disabled, QPalette::WindowText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Button", QPalette::Disabled, QPalette::Button);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ButtonText", QPalette::Disabled, QPalette::ButtonText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Light", QPalette::Disabled, QPalette::Light);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Midlight", QPalette::Disabled, QPalette::Midlight);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Dark", QPalette::Disabled, QPalette::Dark);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Mid", QPalette::Disabled, QPalette::Mid);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Text", QPalette::Disabled, QPalette::Text);
    m_data.append(color);
    color= new PaletteColor(QColor(), "BrightText", QPalette::Disabled, QPalette::BrightText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Base", QPalette::Disabled, QPalette::Base);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Window", QPalette::Disabled, QPalette::Window);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Shadow", QPalette::Disabled, QPalette::Shadow);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Highlight", QPalette::Disabled, QPalette::Highlight);
    m_data.append(color);
    color= new PaletteColor(QColor(), "HighlightedText", QPalette::Disabled, QPalette::HighlightedText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "Link", QPalette::Disabled, QPalette::Link);
    m_data.append(color);
    color= new PaletteColor(QColor(), "LinkVisited", QPalette::Disabled, QPalette::LinkVisited);
    m_data.append(color);
    color= new PaletteColor(QColor(), "AlternateBase", QPalette::Disabled, QPalette::AlternateBase);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ToolTipBase", QPalette::Disabled, QPalette::ToolTipBase);
    m_data.append(color);
    color= new PaletteColor(QColor(), "ToolTipText", QPalette::Disabled, QPalette::ToolTipText);
    m_data.append(color);
    color= new PaletteColor(QColor(), "NoRole", QPalette::Disabled, QPalette::NoRole);
    m_data.append(color);
    color= new PaletteColor(QColor(), "PlaceholderText", QPalette::Disabled, QPalette::PlaceholderText);
    m_data.append(color);
}
int PaletteModel::rowCount(const QModelIndex&) const
{
    return m_data.size();
}
int PaletteModel::columnCount(const QModelIndex&) const
{
    return m_header.size();
}
QVariant PaletteModel::data(const QModelIndex& index, int role) const
{
    if(!index.isValid())
        return QVariant();

    PaletteColor* color= m_data.at(index.row());

    if(Qt::DecorationRole == role)
    {
        if(index.column() == 0)
        {
            return color->getColor();
        }
    }
    else if(Qt::DisplayRole == role)
    {
        if(index.column() == 0)
        {
            return color->getName();
        }
        else
        {
            return m_groupList.at(static_cast<int>(color->getGroup()));
        }
    }
    return QVariant();
}
void PaletteModel::setPalette(QPalette palette)
{
    // dataChanged(QModelIndex());
    beginResetModel();
    for(auto& color : m_data)
    {
        color->setColor(palette.color(color->getGroup(), color->getRole()));
    }
    endResetModel();
}
void PaletteModel::setColor(const QModelIndex& index, QColor color)
{
    m_data[index.row()]->setColor(color);
    emit dataChanged(index, index);
}
QPalette PaletteModel::getPalette()
{
    QPalette palette;
    for(auto& tmp : m_data)
    {
        palette.setColor(tmp->getGroup(), tmp->getRole(), tmp->getColor());
    }
    return palette;
}
void PaletteModel::writeTo(QJsonObject& json)
{
    QJsonArray colors;
    for(auto& tmp : m_data)
    {
        QJsonObject paletteObject;
        tmp->writeTo(paletteObject);
        colors.append(paletteObject);
    }
    json["colors"]= colors;
}
bool PaletteModel::readFrom(const QJsonObject& json)
{
    QJsonArray colors= json["colors"].toArray();

    for(int index= 0; index < colors.size(); ++index)
    {
        QJsonObject paletteObject= colors[index].toObject();
        if((index < m_data.size() && index >= 0))
        {
            m_data[index]->readFrom(paletteObject);
        }
    }
    return true;
}
