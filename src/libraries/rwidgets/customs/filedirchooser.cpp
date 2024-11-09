/*************************************************************************
 *    Copyright (C) 2009 by Renaud Guezennec                             *
 *                                                                       *
 *    https://rolisteam.org/                                          *
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
#include <QFileDialog>
#include <QHBoxLayout>

/**************
 * FileDirChooser *
 **************/

FileDirChooser::FileDirChooser(QWidget* parent, const QString& ext, bool isDirectory)
    : QWidget(parent), m_ext(ext), m_directory(isDirectory)
{
    // Childrens
    m_lineEdit= new QLineEdit(this);
    if(!isDirectory)
        m_lineEdit->setText(QDir::homePath());
    QPushButton* button= new QPushButton(QStringLiteral("..."), this);

    // Layout
    QHBoxLayout* layout= new QHBoxLayout;
    layout->addWidget(m_lineEdit, 1);
    layout->addWidget(button, 0);
    setLayout(layout);

    // Connections
    connect(button, &QPushButton::clicked, this, &FileDirChooser::browse);
    connect(m_lineEdit, &QLineEdit::editingFinished, this, [this]() { emit pathChanged(url()); });

    // Misc
    button->setMaximumWidth(28);
    setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);
}

FileDirChooser::~FileDirChooser()
{
    // QObject should do it right for us already.
}
void FileDirChooser::setMode(bool isDirectory)
{
    m_directory= isDirectory;

    if(!m_directory)
        m_lineEdit->setText(QString());
}

QUrl FileDirChooser::url() const
{
    return m_url;
}

bool FileDirChooser::isValid() const
{
    return m_url.isValid();
}

void FileDirChooser::setExt(const QString& ext)
{
    m_ext= ext;
}

void FileDirChooser::setUrl(const QUrl& path)
{
    m_lineEdit->setText(path.toString());
}

void FileDirChooser::setFilter(const QString& filter)
{
    m_filter= filter;
}

QString FileDirChooser::getFilter()
{
    return m_filter;
}

void FileDirChooser::browse()
{
    if(m_directory)
    {
        m_url= QFileDialog::getExistingDirectory(this, tr("Select directory"), m_lineEdit->text(),
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    }
    else
    {
        m_url= QFileDialog::getSaveFileUrl(this, tr("Open File"), m_lineEdit->text(), m_filter);
    }

    auto path= m_url.path();

    if(!path.isEmpty())
    {
        if(!path.endsWith(m_ext) && !m_ext.isEmpty())
            m_url.setPath(path.append(m_ext));
        m_lineEdit->setText(m_url.toString());
        emit pathChanged(url());
    }
}
