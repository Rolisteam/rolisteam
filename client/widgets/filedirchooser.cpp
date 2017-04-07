/*************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *                                                                       *
 *    http://www.rolisteam.org/                                          *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/
#include "filedirchooser.h"

#include <QDir>
#include <QHBoxLayout>
#include <QFileDialog>

/**************
 * FileDirChooser *
 **************/

FileDirChooser::FileDirChooser(bool isDirectory ,QWidget * parent)
 : QWidget(parent),m_directory(isDirectory)
{


    // Childrens
    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setText(QDir::homePath());
    QPushButton * button = new QPushButton(QStringLiteral("..."), this);

    // Layout
    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget(m_lineEdit, 1);
    layout->addWidget(button, 0);
    setLayout(layout);

    // Connections
    connect(button, SIGNAL(clicked()), this, SLOT(browse()));
    connect(m_lineEdit,SIGNAL(editingFinished()),this,SIGNAL(pathChanged()));

    // Misc
    button->setMaximumWidth(28);
    setContentsMargins(0,0,0,0);
    layout->setContentsMargins(0,0,0,0);
}

FileDirChooser::~FileDirChooser()
{
    // QObject should do it right for us already.
}
void FileDirChooser::setMode(bool isDirectory)
{
    m_directory=isDirectory;
}

void FileDirChooser::setPath(const QString & path)
{
    m_lineEdit->setText(path);
}

QString FileDirChooser::path() const
{
    return m_lineEdit->text();
}
void FileDirChooser::setFilter(const QString& filter)
{
    m_filter=filter;
}

QString FileDirChooser::getFilter()
{
    return m_filter;
}

void FileDirChooser::browse()
{
     QString result;
    if(m_directory)
    {
    result = QFileDialog::getExistingDirectory(this,
            tr("Select directory"),
            m_lineEdit->text(),
            QFileDialog::ShowDirsOnly  | QFileDialog::DontResolveSymlinks);
    }
    else
    {
        result = QFileDialog::getOpenFileName(this,
                tr("Open File"),
                m_lineEdit->text(),
                m_filter );
    }
    if (!result.isEmpty())
    {
        m_lineEdit->setText(result);
        emit pathChanged();
    }
}
