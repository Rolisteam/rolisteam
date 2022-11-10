/***************************************************************************
 *	Copyright (C) 2018 by Renaud Guezennec                                 *
 *   https://rolisteam.org/contact                                      *
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
#include <QMenu>
#include <QPointer>
#include <QString>
#include <QWidget>
#include <memory>

#include "customs/workspace.h"
#include "preferences/preferencesmanager.h"

#include "rwidgets/mediacontainers/mediacontainer.h"
Q_DECLARE_LOGGING_CATEGORY(MediaContainerPDF)
class NetworkLink;
class NetworkMessageWriter;
class QPdfWidget;
class Overlay;
class PdfController;
class QPdfDocument;
class QPdfView;
namespace Ui
{
class PdfViewer;
}
/**
 * @brief The Image class displays image to the screen. It also manages image from Internet and the zooming.
 */
class PdfViewer : public MediaContainer
{
    Q_OBJECT
public:
    enum ZoomLevelValue
    {
        FitWidth,
        FitInView,
        TwentyFiveZoom,
        FiftyZoom,
        SeventyFiveZoom,
        OneHundredZoom,
        OneHundredFitfyZoom,
        TwoHundredZoom
    };
    Q_ENUM(ZoomLevelValue)
    PdfViewer(PdfController* ctrl, QWidget* parent= nullptr);
    /**
     * @brief ~PdfViewer destructor.
     */
    virtual ~PdfViewer();

    void savePdfToFile(QFile& file);
    void savePdfToFile(QDataStream& out);
    void setParent(Workspace* parent);
    void contextMenuEvent(QContextMenuEvent* event) override;

protected:
    void makeConnections();
    bool eventFilter(QObject* obj, QEvent* event) override;

protected slots:
    void extractImage();
    void extractText();
    void extractMap();
    void showOverLay();
    void sharePdfTo();
    void updateTitle();
    void bookmarkSelected(const QModelIndex& index);

private:
    Ui::PdfViewer* m_ui= nullptr;
    std::unique_ptr<QPdfDocument> m_document;
    QPointer<PdfController> m_pdfCtrl;
    std::unique_ptr<Overlay> m_overlay;
};

#endif
