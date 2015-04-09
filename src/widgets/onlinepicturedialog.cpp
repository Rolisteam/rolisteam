/***************************************************************************
* Copyright (C) 2015 by Renaud Guezennec                                   *
* http://renaudguezennec.homelinux.org/accueil,3.html                      *
*                                                                          *
*  This file is part of rcm                                                *
*                                                                          *
* Rolisteam is free software; you can redistribute it and/or modify              *
* it under the terms of the GNU General Public License as published by     *
* the Free Software Foundation; either version 2 of the License, or        *
* (at your option) any later version.                                      *
*                                                                          *
* This program is distributed in the hope that it will be useful,          *
* but WITHOUT ANY WARRANTY; without even the implied warranty of           *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
* GNU General Public License for more details.                             *
*                                                                          *
* You should have received a copy of the GNU General Public License        *
* along with this program; if not, write to the                            *
* Free Software Foundation, Inc.,                                          *
* 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
***************************************************************************/
#include <QUrl>
#include <QNetworkReply>
#include <QFileInfo>
#include <QNetworkRequest>

#include "onlinepicturedialog.h"
#include "ui_onlinepicturedialog.h"

OnlinePictureDialog::OnlinePictureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OnlinePictureDialog)
{
    ui->setupUi(this);
    connect(ui->m_urlField,SIGNAL(editingFinished()),this,SLOT(uriChanged()));
    connect(ui->m_downloadPush,SIGNAL(clicked()),this,SLOT(uriChanged()));
    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
}

OnlinePictureDialog::~OnlinePictureDialog()
{
    delete ui;
}
void OnlinePictureDialog::uriChanged()
{
    QFileInfo info(ui->m_urlField->text());
    if(ui->m_titleLineEdit->text().isEmpty())
    {
        ui->m_titleLineEdit->setText(info.baseName());
    }
   m_manager->get(QNetworkRequest(QUrl(ui->m_urlField->text())));
}
void OnlinePictureDialog::replyFinished(QNetworkReply* reply)
{
    QByteArray data = reply->readAll();
    m_pix.loadFromData(data);
    ui->m_imageViewerLabel->setPixmap(m_pix);
}
QString OnlinePictureDialog::getPath()
{
    return ui->m_urlField->text();
}

QPixmap OnlinePictureDialog::getPixmap()
{
    return m_pix;
}
QString OnlinePictureDialog::getTitle()
{
    return ui->m_titleLineEdit->text();
}
