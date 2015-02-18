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
#include "pathchooser.h"
#include <QFileDialog>

PathChooser::PathChooser(QWidget *parent) :
    QWidget(parent)
{
    m_isDirectory=true;
    m_button = new QPushButton(tr("Browse"));
    connect(m_button,SIGNAL(pressed()),this,SLOT(onButton()));
    m_line= new QLineEdit();
    connect(m_line,SIGNAL(textChanged(QString)),this,SLOT(editPath()));
    m_label = new QLabel();

    setupUi();
}

void PathChooser::setTitle(QString& t)
{
    m_title=t;
}

void PathChooser::setFilters(QString& t)
{
    m_filters=t;
}
void PathChooser::activeDirectory(bool m)
{
    m_isDirectory=m;
}

void PathChooser::setPath(QString m)
{
    m_path = m;
    m_line->setText(m_path);
}
void PathChooser::setupUi()
{
    m_layout = new QHBoxLayout;
    m_layout->setMargin(0);
    m_layout->addWidget(m_line);
    m_layout->addWidget(m_button);
    m_layout->addWidget(m_label);


    setLayout(m_layout);

}
void PathChooser::editPath()
{
    m_path =m_line->text();

    emit pathChanged();
}

void PathChooser::onButton()
{
    if(m_isDirectory)
        m_path = QFileDialog::getExistingDirectory(this, m_title, m_path);
    else
        m_path = QFileDialog::getOpenFileName(this, m_title,m_path,m_filters);

    m_line->setText(m_path);
    emit pathChanged();
}

QString PathChooser::getPath()
{
    return m_path;
}
void PathChooser::checkPermission()
{
    QFileInfo file(m_path);
    if(file.isWritable())
    {
        m_label->setPixmap();
    }
    else
    {
        m_label->setPixmap();
    }
}
