/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
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

#include "aboutrolisteam.h"
#include "ui_aboutrolisteam.h"

AboutRolisteam::AboutRolisteam(QString version,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutRolisteam)
{
    ui->setupUi(this);
    ui->m_textLabel->setText(QString("Rolisteam v%1").arg(version));

    QString table="<table>";
    QString line("<tr><td><a href='%3'>%1</td><td>(%2)</td></tr>");
    table+=line.arg("Renaud Guezennec").arg(tr("English & French")).arg("https://www.transifex.com/accounts/profile/Le_Sage/");
    table+=line.arg("Lorram Lomeu de Souza Rampi").arg(tr("Portuguese")).arg("https://www.transifex.com/accounts/profile/lorrampi/");
    table+=line.arg("Juliana Alves de Sousa Rampi").arg(tr("Portuguese")).arg("https://www.transifex.com/accounts/profile/JuAlves/");
    table+=line.arg("Gergely Krekó").arg(tr("Hungarian")).arg("https://www.transifex.com/accounts/profile/KrekoG/");
    table+=line.arg("Kaya Zeren").arg(tr("Turkish")).arg("https://www.transifex.com/accounts/profile/kayazeren/");
    table+=line.arg("Alexia Béné").arg(tr("French & German")).arg("https://www.transifex.com/accounts/profile/IGrumoI/");
    table+=line.arg("Baadur Jobava").arg(tr("Romanian")).arg("https://www.transifex.com/accounts/profile/Jobava/");
    table+=line.arg("Jesus M").arg(tr("Spanish")).arg("https://www.transifex.com/accounts/profile/omik/");
    table+=line.arg("Heimen Stoffels").arg(tr("Dutch")).arg("https://www.transifex.com/accounts/profile/Vistaus/");
    table+=line.arg("Xavier Gil").arg(tr("Catalan")).arg("https://www.transifex.com/user/profile/Xavierg63/");
    table+="</table>";

    ui->m_textBrowser->setHtml(QString("<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body><p>%1</p>\
                               <p>%2</p>\
                               <p>%3</p>\
                               <ul>\
                                 <li><a href='http://www.rolisteam.org/'>%4</a></li>\
                                 <li><a href='https://github.com/Rolisteam/rolisteam/issues'>%5</a></li>\
                               </ul>\
                              <p>%6 :</p><ul><li><a href='http://www.rolisteam.org/contact'>Renaud Guezennec</a></li></ul>\
                              <p>%7 :</p><ul><li><a href='mailto:joseph.boudou@matabio.net'>Joseph Boudou</a></li><li><a href='mailto:rolistik@free.fr'>Romain Campioni</a></li></ul>\
                              <p>%8 :</p>"
                              "%9")
            .arg(tr("Rolisteam helps you to manage a tabletop role playing game with remote friends/players. It provides many features to share maps, pictures and it also includes tool to communicate with your friends/players. The goal is to make Rolisteam-managed RPG games as good as RPG games around your table. To achieve it, we are working hard to provide you more and more features. Existing features : Map sharing (with permission management), Image sharing, background music, dice roll and so on. Rolisteam is written in Qt5"))
            .arg(tr("You may modify and redistribute the program under the terms of the GPL (version 2 or later). A copy of the GPL is contained in the 'COPYING' file distributed with Rolisteam. Rolisteam is copyrighted by its contributors. See the 'COPYRIGHT' file for the complete list of contributors. We provide no warranty for this program."))
            .arg(tr("Web Sites:"))
            .arg(tr("Official Rolisteam Site"))
            .arg(tr("Bug Tracker"))
            .arg(tr("Current developers")).arg(tr("Contributors")).arg(tr("Translators")).arg(table));
}

AboutRolisteam::~AboutRolisteam()
{
    delete ui;
}
