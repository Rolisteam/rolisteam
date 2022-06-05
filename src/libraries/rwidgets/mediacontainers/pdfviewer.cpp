/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
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

#include "pdfviewer.h"
#include <QBuffer>
#include <QDebug>
#include <QPdfBookmarkModel>
#include <QPdfDocument>
#include <QPdfPageNavigation>
#include <QPdfView>
#include <QScrollBar>
#include <QShortcut>
#include <QtGui>

#include "controller/view_controller/pdfcontroller.h"
#include "customs/overlay.h"
#include "ui_pdfviewer.h"

#define MARGING 0
#define ICON_SIZE 32

PdfViewer::PdfViewer(PdfController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::PDFContainer, parent)
    , m_ui(new Ui::PdfViewer)
    , m_document(new QPdfDocument())
    , m_pdfCtrl(ctrl)
{
    setObjectName("PdfViewer");
    setWindowIcon(QIcon::fromTheme("pdfLogo"));
    auto wid= new QWidget();
    m_ui->setupUi(wid);
    setWidget(wid);

    m_ui->m_view->setDocument(m_document.get());

    QPdfBookmarkModel* bookmarkModel= new QPdfBookmarkModel(this);
    m_ui->bookmarkView->setModel(bookmarkModel);
    bookmarkModel->setDocument(m_document.get());

    makeConnections();
    if(m_pdfCtrl)
    {
        auto buf= m_pdfCtrl->buffer();
        if(buf->open(QIODevice::ReadOnly))
            m_document->load(buf);
    }
}

PdfViewer::~PdfViewer() {}

void PdfViewer::makeConnections()
{
    m_ui->m_cropViewAct->setIcon(QIcon::fromTheme("crop"));
    m_ui->m_shareAct->setIcon(QIcon::fromTheme("document-share"));

    m_ui->m_cropBtn->setDefaultAction(m_ui->m_cropViewAct);
    m_ui->m_shareBtn->setDefaultAction(m_ui->m_shareAct);
    m_ui->m_exportToMapBtn->setDefaultAction(m_ui->m_exportToMapAct);
    m_ui->m_exportToImageBtn->setDefaultAction(m_ui->m_exportToImage);
    m_ui->m_continuousBtn->setDefaultAction(m_ui->m_continuousAct);

    m_ui->m_zoomInBtn->setDefaultAction(m_ui->m_zoomInAct);
    m_ui->m_zoomOutBtn->setDefaultAction(m_ui->m_zoomOutAct);

    m_ui->m_nextPageBtn->setDefaultAction(m_ui->m_nextPageAct);
    m_ui->m_previousPageBtn->setDefaultAction(m_ui->m_previousPageAct);
    // m_ui->m_shareBtn->setDefaultAction(m_ui->m_shareAct);

    connect(m_ui->m_continuousAct, &QAction::triggered, this, []() {
        // m_ui->m_view, &QPdfView::setZoomMode;
    });

    connect(m_ui->m_continuousAct, &QAction::triggered, this, [this](bool continuous) {
        m_ui->m_view->setPageMode(continuous ? QPdfView::MultiPage : QPdfView::SinglePage);
    });

    connect(m_ui->m_zoomInAct, &QAction::triggered, this, [this]() { m_pdfCtrl->zoomIn(); });

    connect(m_ui->m_zoomOutAct, &QAction::triggered, this, [this]() { m_pdfCtrl->zoomOut(); });

    m_ui->m_continuousAct->setChecked(true);
    m_ui->m_view->setPageMode(QPdfView::MultiPage);

    connect(m_pdfCtrl, &PdfController::zoomFactorChanged, m_ui->m_view, &QPdfView::setZoomFactor);

    connect(m_ui->bookmarkView, &QTreeView::activated, this, &PdfViewer::bookmarkSelected);

    auto nav= m_ui->m_view->pageNavigation();
    connect(nav, &QPdfPageNavigation::currentPageChanged, this, [nav, this](int page) {
        m_ui->lineEdit->setText(tr("%1/%2", "Current page/ total page").arg(page).arg(nav->pageCount()));
    });
    connect(nav, &QPdfPageNavigation::canGoToNextPageChanged, m_ui->m_nextPageAct, &QAction::setEnabled);
    connect(nav, &QPdfPageNavigation::canGoToPreviousPageChanged, m_ui->m_previousPageAct, &QAction::setEnabled);

    connect(m_ui->m_nextPageAct, &QAction::triggered, nav, &QPdfPageNavigation::goToNextPage);
    connect(m_ui->m_previousPageAct, &QAction::triggered, nav, &QPdfPageNavigation::goToPreviousPage);

    for(auto act : m_cropOption)
    {
        // act->setVisible(false);
    }
    /*m_cropCurrentView= new QAction(tr("Crop Current View"), this);
    m_cropCurrentView->setIcon(QIcon(":/resources/images/crop.svg"));

    connect(m_cropCurrentView, &QAction::triggered, this, &PdfViewer::showOverLay);

    m_shareDocument= new QAction(tr("Document to all"), this);
    m_shareDocument->setIcon(QIcon(":/resources/images/document-share.svg"));

    m_toVmap= new QAction(tr("Export into VMap"), this);
    m_toVmap->setData(QVariant::fromValue(Core::ContentType::VECTORIALMAP));

    m_image= new QAction(tr("Export as Image"), this);
    m_image->setData(QVariant::fromValue(Core::ContentType::PICTURE));

    connect(m_toVmap, &QAction::triggered, this, &PdfViewer::exportImage);
    connect(m_image, &QAction::triggered, this, &PdfViewer::exportImage);
    connect(m_shareDocument, &QAction::triggered, this, &PdfViewer::sharePdfTo);*/
}

void PdfViewer::exportImage()
{
    if(!m_overlay)
        return;

    auto act= qobject_cast<QAction*>(sender());
    auto type= static_cast<Core::ContentType>(act->data().toInt());

    auto rect= m_overlay->rect();
    auto pix= m_ui->m_view->grab(rect);

    // emit openImageAs(pix, type);
    switch(type)
    {
    case Core::ContentType::VECTORIALMAP:
        m_pdfCtrl->shareImageIntoVMap(pix);
        break;
    case Core::ContentType::PICTURE:
        m_pdfCtrl->shareImageIntoVMap(pix);
        break;
    default:
        break;
    }

    m_overlay.reset();
    // m_cropCurrentView->setChecked(false);
}

void PdfViewer::sharePdfTo()
{
    auto answer
        = QMessageBox::question(this, tr("Sharing Pdf File"),
                                tr("PDF transfert can be really heavy.\nDo you want to continue and share the PDF?"),
                                QMessageBox::Yes | QMessageBox::Cancel);
    if(answer == QMessageBox::Yes)
    {
        m_pdfCtrl->shareAsPdf();
    }
}

void PdfViewer::updateTitle()
{
    setWindowTitle(tr("%1 - (PDF)").arg(m_pdfCtrl->name()));
}

void PdfViewer::bookmarkSelected(const QModelIndex& index)
{
    if(!index.isValid())
        return;

    const int page= index.data(QPdfBookmarkModel::PageNumberRole).toInt();
    m_ui->m_view->pageNavigation()->setCurrentPage(page);
}

void PdfViewer::showOverLay()
{
    /*if(m_cropCurrentView->isChecked())
    {
        if(m_overlay == nullptr)
        {
            m_overlay= new Overlay(m_ui->m_view->geometry(), m_ui->m_view->parentWidget());
        }
        m_overlay->setGeometry(m_ui->m_view->geometry());
        m_overlay->show();
        for(auto act : m_cropOption)
        {
            act->setVisible(true);
        }
    }
    else
    {
        if(m_overlay != nullptr)
        {
            m_overlay->hide();
            delete m_overlay;
            m_overlay= nullptr;
        }
        for(auto act : m_cropOption)
        {
            act->setVisible(false);
        }
    }*/
}

void PdfViewer::savePdfToFile(QDataStream& out)
{
    Q_UNUSED(out)
    //    QByteArray baPdfViewer;
    //    QBuffer bufPdfViewer(&baPdfViewer);
    //    if(!m_pixMap.isNull())
    //    {
    //        if (!m_pixMap.save(&bufPdfViewer, "jpg", 70))
    //        {
    //            error(tr("PdfViewer Compression fails (savePdfViewerToFile - PdfViewer.cpp)"),this);
    //            return;
    //        }
    //        out << baPdfViewer;
    //    }
}

void PdfViewer::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    // menu.addAction(m_cropCurrentView);
    // menu.addAction(m_shareDocument);
    if(m_overlay != nullptr)
    {
        menu.addSeparator();
        for(auto act : m_cropOption)
        {
            menu.addAction(act);
        }
    }

    menu.exec(event->globalPos());
}

/*void PdfViewer::putDataIntoCleverUri()
{
   QByteArray data;
   QDataStream out(&data, QIODevice::WriteOnly);
   savePdfToFile(out);
   if(nullptr != m_uri)
   {
       m_uri->setData(data);
   }
}*/
