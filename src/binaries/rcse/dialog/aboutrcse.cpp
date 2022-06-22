/***************************************************************************
* Copyright (C) 2018 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
*                                                                          *
*  This file is part of rcse                                               *
*                                                                          *
* rcse is free software; you can redistribute it and/or modify             *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* rcse is distributed in the hope that it will be useful,                  *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#include "aboutrcse.h"
#include "ui_aboutrcse.h"

AboutRcse::AboutRcse(QString version, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutRcse),
    m_version(version)
{
    ui->setupUi(this);


    QString text("Rolisteam Character Sheet Editor v%1<br/><br/>"
                 "This software is dedicated to create character sheet for Rolisteam v%2 and more.<br/>"
                 "Please check the documentation at: wiki.rolisteam.org");
    ui->label->setText(text.arg(m_version).arg(m_version));
}

AboutRcse::~AboutRcse()
{
    delete ui;
}

QString AboutRcse::version() const
{
    return m_version;
}

void AboutRcse::setVersion(const QString &version)
{
    m_version = version;
}
