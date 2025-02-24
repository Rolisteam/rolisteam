/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                                      *
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

#include "aboutrolisteam.h"
#include "common/logcategory.h"
#include "ui_aboutrolisteam.h"
#include <inja.hpp>
#include <json.hpp>

#include <QFile>

namespace jsonkey
{
constexpr auto description{"description"};
constexpr auto description2{"description2"};
constexpr auto websites{"websites"};
constexpr auto official{"official"};
constexpr auto tracker{"bugtracker"};

} // namespace jsonkey

AboutRolisteam::AboutRolisteam(QString version, QWidget* parent) : QDialog(parent), ui(new Ui::AboutRolisteam)
{
    ui->setupUi(this);
    ui->m_textLabel->setText(QString("Rolisteam v%1").arg(version));

    inja::json data;
    data[jsonkey::description]
        = tr("Rolisteam helps you to manage a tabletop role playing game with remote friends/players. It "
             "provides many features to share maps, pictures and it also includes tool to communicate with your "
             "friends/players. The goal is to make Rolisteam-managed RPG games as good as RPG games around your "
             "table. To achieve it, we are working hard to provide you more and more features. Existing "
             "features : Map sharing (with permission management), Image sharing, background music, dice roll "
             "and so on. Rolisteam is written with Qt")
              .toStdString();
    data[jsonkey::description2]
        = tr("You may modify and redistribute the program under the terms of the GPL (version 2 or later). A "
             "copy of the GPL is contained in the 'COPYING' file distributed with Rolisteam. Rolisteam is "
             "copyrighted by its contributors. See the 'COPYRIGHT' file for the complete list of contributors. "
             "We provide no warranty for this program.")
              .toStdString();
    data[jsonkey::official]= tr("Official Rolisteam Site").toStdString();
    data[jsonkey::websites]= tr("Web Sites:").toStdString();
    data[jsonkey::tracker]= tr("Bug Tracker").toStdString();
    QFile file(":/resources/rolisteam/templates/about.template");
    if(!file.open(QIODevice::ReadOnly))
    {
        qCWarning(RolisteamCat) << tr("Impossible to read the template.");
        return;
    }

    auto byte= file.readAll();

    try
    {
        ui->m_textBrowser->setHtml(QString::fromStdString(inja::render(byte.toStdString(), data)));
    }
    catch(const inja::RenderError& e)
    {
        qCWarning(RolisteamCat) << e.message;
        return;
    }
}

AboutRolisteam::~AboutRolisteam()
{
    delete ui;
}
