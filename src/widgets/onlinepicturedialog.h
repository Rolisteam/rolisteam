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
#include <QLabel>
#include <QNetworkAccessManager>
namespace Ui {
class OnlinePictureDialog;
}
/**
 * @brief The OnlinePictureDialog class
 */
class OnlinePictureDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief OnlinePictureDialog
     * @param parent
     */
    explicit OnlinePictureDialog(QWidget *parent = 0);
    /**
     * @brief ~OnlinePictureDialog
     */
    virtual ~OnlinePictureDialog();
    /**
     * @brief getPath
     * @return
     */
    QString getPath();
    /**
     * @brief getPixmap
     * @return
     */
    QPixmap getPixmap();
    /**
     * @brief getTitle
     * @return
     */
    QString getTitle();
private slots:
    /**
     * @brief uriChanged
     */
    void uriChanged();
    /**
     * @brief replyFinished
     * @param reply
     */
    void replyFinished(QNetworkReply* reply);

protected:
    /**
     * @brief resizeLabel
     */
    void resizeLabel();
    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent *event);

private:
    Ui::OnlinePictureDialog* ui;
    QNetworkAccessManager* m_manager;
    QPixmap m_pix;
    double m_zoomLevel;
    QLabel* m_imageViewerLabel;
};

#endif // ONLINEPICTUREDIALOG_H
