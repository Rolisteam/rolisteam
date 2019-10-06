/***************************************************************************
 *	Copyright (C) 2018 by Renaud Guezennec                                 *
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

#ifndef PDFVIEWER_H
#define PDFVIEWER_H

#include <QAction>
#include <QFile>
#include <QLabel>
#include <QString>
#include <QWidget>

#include "preferences/preferencesmanager.h"
#include "widgets/improvedworkspace.h"

#include "data/mediacontainer.h"

class NetworkLink;
class NetworkMessageWriter;
class QPdfWidget;
class Overlay;
/**
 * @brief The Image class displays image to the screen. It also manages image from Internet and the zooming.
 */
class PdfViewer : public MediaContainer
{
    Q_OBJECT

public:
    PdfViewer(ImprovedWorkspace* parent= nullptr);
    /**
     * @brief ~PdfViewer destructor.
     */
    virtual ~PdfViewer();

    void savePdfToFile(QFile& file);
    void savePdfToFile(QDataStream& out);
    void setParent(ImprovedWorkspace* parent);

    virtual bool readFileFromUri();
    virtual bool openMedia();
    virtual void saveMedia();

    virtual void putDataIntoCleverUri();

    virtual void fill(NetworkMessageWriter& msg);
    virtual void readMessage(NetworkMessageReader& msg);

    void contextMenuEvent(QContextMenuEvent* event);
signals:
    void openImageAs(const QPixmap&, CleverURI::ContentType);

protected:
    void createActions();
    void creationToolBar();
protected slots:
    void showOverLay();
    void exportImage();
    void sharePdfTo();
    void updateTitle();

private:
    QString m_owner;
    QVBoxLayout* m_mainLayout= nullptr;

    QAction* m_cropCurrentView= nullptr;
    QAction* m_shareDocument= nullptr;

    QAction* m_toMap= nullptr;
    QAction* m_toVmap= nullptr;
    QAction* m_image= nullptr;

    std::vector<QAction*> m_cropOption;

    QPdfWidget* m_pdfWidget= nullptr;
    Overlay* m_overlay= nullptr;
};

#endif
