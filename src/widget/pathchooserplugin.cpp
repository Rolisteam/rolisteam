/***************************************************************************
    *	Copyright (C) 2011 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify  *
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
#include "pathchooserplugin.h"
#include "pathchooser.h"
#include <QtPlugin>

PathChooserPlugin::PathChooserPlugin(QObject *parent) :
    QObject(parent)
{
    initialized = false;
}



void PathChooserPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;
    
    initialized = true;
}

bool PathChooserPlugin::isInitialized() const
{
    return initialized;
}

QWidget *PathChooserPlugin::createWidget(QWidget *parent)
{
    return new PathChooser(parent);
}

QString PathChooserPlugin::name() const
{
    return "PathChooser";
}

QString PathChooserPlugin::group() const
{
    return "Display Widgets [Examples]";
}

QIcon PathChooserPlugin::icon() const
{
    return QIcon();
}

QString PathChooserPlugin::toolTip() const
{
    return "";
}

QString PathChooserPlugin::whatsThis() const
{
    return "";
}

bool PathChooserPlugin::isContainer() const
{
    return false;
}

QString PathChooserPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
            " <widget class=\"PathChooser\" name=\"PathChooser\">\n"
            "  <property name=\"geometry\">\n"
            "   <rect>\n"
            "    <x>0</x>\n"
            "    <y>0</y>\n"
            "    <width>100</width>\n"
            "    <height>25</height>\n"
            "   </rect>\n"
            "  </property>\n"
            "  <property name=\"toolTip\" >\n"
            "   <string>The current time</string>\n"
            "  </property>\n"
            "  <property name=\"whatsThis\" >\n"
            "   <string>The analog clock widget displays the current time.</string>\n"
            "  </property>\n"
            " </widget>\n"
            "</ui>\n";
}

QString PathChooserPlugin::includeFile() const
{
    return "pathchooser.h";
}

//Q_EXPORT_PLUGIN2(pathchooserplugin, PathChooserPlugin)
