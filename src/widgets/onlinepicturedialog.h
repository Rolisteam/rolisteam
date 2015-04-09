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
#ifndef ONLINEPICTUREDIALOG_H
#define ONLINEPICTUREDIALOG_H

#include <QDialog>
#include <QNetworkAccessManager>
namespace Ui {
class OnlinePictureDialog;
}

class OnlinePictureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OnlinePictureDialog(QWidget *parent = 0);
    ~OnlinePictureDialog();
    QString getPath();
    QPixmap getPixmap();
    QString getTitle();
private slots:
    void uriChanged();
    void replyFinished(QNetworkReply* reply);

private:
    Ui::OnlinePictureDialog* ui;
    QNetworkAccessManager* m_manager;
    QPixmap m_pix;
};

#endif // ONLINEPICTUREDIALOG_H
