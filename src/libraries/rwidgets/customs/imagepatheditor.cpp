/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "imagepatheditor.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QStyle>

#include "worker/iohelper.h"

namespace rwidgets
{

ImagePathEditor::ImagePathEditor(const QString& root, QWidget* parent) : QWidget(parent), m_root(root)
{
    setUi();
}

ImagePathEditor::~ImagePathEditor() {}

QString ImagePathEditor::filename() const
{
    return m_filename;
}

void ImagePathEditor::focusInEvent(QFocusEvent* event)
{
    QWidget::focusInEvent(event);
}

void ImagePathEditor::setUi()
{
    QHBoxLayout* hbox= new QHBoxLayout();
    hbox->setMargin(0);
    hbox->setSpacing(0);

    setLayout(hbox);

    m_photoBrowser= new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), "");

    m_photoEdit= new QLineEdit();

    m_cleanButton= new QPushButton(QIcon::fromTheme("remove"), "");

    hbox->addWidget(m_photoEdit, 1);
    hbox->addWidget(m_photoBrowser);
    hbox->addWidget(m_cleanButton);

    connect(m_photoBrowser, &QPushButton::pressed, this, &ImagePathEditor::openFile);
    connect(m_photoEdit, &QLineEdit::textEdited, this, &ImagePathEditor::readPixmap);
    connect(m_cleanButton, &QPushButton::pressed, this, &ImagePathEditor::clearPixmap);
}

void ImagePathEditor::setPixmap(QPixmap str)
{
    m_pixmap= str;
}

QPixmap ImagePathEditor::getData() const
{
    return m_pixmap;
}

void ImagePathEditor::setFilename(const QString& file)
{
    if(m_filename == file)
        return;
    m_filename= file;
    emit fileNameChanged();
}

void ImagePathEditor::clearPixmap()
{
    setFilename(QString());
    m_pixmap= QPixmap();
}

void ImagePathEditor::openFile()
{
    QString fileName= QFileDialog::getOpenFileName(this, tr("Get picture for Character State"), m_root,
                                                   "*.jpg *jpeg *.png *.bmp *.svg");
    if(!fileName.isEmpty())
    {
        if(!fileName.startsWith(m_root))
            fileName= IOHelper::copyFile(fileName, m_root);
        QFileInfo info(fileName);
        if(info.exists())
        {
            setFilename(fileName);
            readPixmap(fileName);
        }
        // readPixmap(m_root);
        // setFilename(fileName);
        // readPixmap(fileName);
    }
}
void ImagePathEditor::readPixmap(const QString& str)
{
    if(str.isEmpty())
        return;

    QPixmap pix(str);
    if(!pix.isNull())
    {
        m_pixmap= pix;
        m_photoEdit->setText(str);
    }
}
} // namespace rwidgets
