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
#include <QMessageBox>
#include <QPdfBookmarkModel>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QPdfView>
#include <QScrollBar>
#include <QShortcut>
#include <type_traits>

#include "controller/view_controller/pdfcontroller.h"
#include "customs/overlay.h"
#include "ui_pdfviewer.h"

#define MARGING 0
#define ICON_SIZE 32

Q_LOGGING_CATEGORY(MediaContainerPDF, "mediacontainer.pdf")

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
        if(!m_pdfCtrl->data().isEmpty())
        {
            auto buf= m_pdfCtrl->buffer();
            if(buf->open(QIODevice::ReadOnly))
                m_document->load(buf);
        }
        else
            m_document->load(m_pdfCtrl->url().toLocalFile());
    }
}

PdfViewer::~PdfViewer() {}

void PdfViewer::makeConnections()
{
    m_ui->m_cropViewAct->setIcon(QIcon::fromTheme("crop"));
    m_ui->m_shareAct->setIcon(QIcon::fromTheme("share_doc"));
    m_ui->m_continuousAct->setIcon(QIcon::fromTheme("view-page-continuous"));
    m_ui->m_exportToMapAct->setIcon(QIcon::fromTheme("edit-copy"));
    m_ui->m_exportToImage->setIcon(QIcon::fromTheme("image-x-generic"));
    m_ui->m_extractTextAct->setIcon(QIcon::fromTheme("text"));

    m_ui->m_cropBtn->setDefaultAction(m_ui->m_cropViewAct);
    m_ui->m_shareBtn->setDefaultAction(m_ui->m_shareAct);
    m_ui->m_exportToMapBtn->setDefaultAction(m_ui->m_exportToMapAct);
    m_ui->m_exportToImageBtn->setDefaultAction(m_ui->m_exportToImage);
    m_ui->m_continuousBtn->setDefaultAction(m_ui->m_continuousAct);

    m_ui->m_extractTextBtn->setDefaultAction(m_ui->m_extractTextAct);
    m_ui->m_extractTextBtn->setVisible(false);
    m_ui->m_exportToImageBtn->setVisible(false);

    m_ui->m_zoomInBtn->setDefaultAction(m_ui->m_zoomInAct);
    m_ui->m_zoomOutBtn->setDefaultAction(m_ui->m_zoomOutAct);

    m_ui->m_nextPageBtn->setDefaultAction(m_ui->m_nextPageAct);
    m_ui->m_previousPageBtn->setDefaultAction(m_ui->m_previousPageAct);

    m_ui->m_exportToImage->setEnabled(false);
    m_ui->m_exportToMapAct->setEnabled(false);
    m_ui->m_extractTextAct->setEnabled(false);

    connect(m_ui->m_zoomLevel, &QComboBox::currentIndexChanged, this,
            [this]()
            {
                auto i= m_ui->m_zoomLevel->currentIndex();
                //, FitHeight, TwentyFiveZoom, FiftyZoom, SeventyFiveZoom, OneHundredZoom, OneHundredFitfyZoom,
                // TwoHundredZoom};
                if(i == FitWidth)
                {
                    m_ui->m_view->setZoomMode(QPdfView::ZoomMode::FitToWidth);
                }
                else if(i == FitInView)
                {
                    m_ui->m_view->setZoomMode(QPdfView::ZoomMode::FitInView);
                }
                else
                {
                    m_ui->m_view->setZoomMode(QPdfView::ZoomMode::Custom);

                    static QHash<int, qreal> dataValue{{TwentyFiveZoom, 0.25},     {FiftyZoom, 0.5},
                                                       {SeventyFiveZoom, 0.75},    {OneHundredZoom, 1.0},
                                                       {OneHundredFitfyZoom, 1.5}, {TwoHundredZoom, 2.0}};
                    m_ui->m_view->setZoomFactor(dataValue.value(i));
                }
            });
    connect(m_ui->m_zoomLevel, &QComboBox::currentTextChanged, this,
            [this](const QString& text)
            {
                bool ok;
                auto zoomlvl= static_cast<qreal>(text.toInt(&ok));
                if(ok)
                {
                    m_ui->m_view->setZoomFactor(zoomlvl / 100.0);
                }
            });

    m_ui->m_zoomLevel->setValidator(new QIntValidator);
    connect(m_ui->m_continuousAct, &QAction::triggered, this,
            [this](bool continuous) {
                m_ui->m_view->setPageMode(continuous ? QPdfView::PageMode::MultiPage : QPdfView::PageMode::SinglePage);
            });

    const auto& rect= geometry();
    m_ui->m_splitter->setSizes(
        QList{{static_cast<int>(rect.width() * 0.2), static_cast<int>(rect.width() * 0.8)}}); // QSplitter
    connect(m_ui->m_zoomInAct, &QAction::triggered, this, [this]() { m_pdfCtrl->zoomIn(); });
    connect(m_ui->m_zoomOutAct, &QAction::triggered, this, [this]() { m_pdfCtrl->zoomOut(); });

    m_ui->m_continuousAct->setChecked(true);
    m_ui->m_view->setPageMode(QPdfView::PageMode::MultiPage);

    connect(m_pdfCtrl, &PdfController::zoomFactorChanged, m_ui->m_view, &QPdfView::setZoomFactor);

    connect(m_ui->bookmarkView, &QTreeView::activated, this, &PdfViewer::bookmarkSelected);

    auto nav= m_ui->m_view->pageNavigator();
    auto updateCurrentPage= [this, nav]()
    {
        if(!nav)
            return;

        m_ui->m_curentPage->setValue(nav->currentPage() + 1); //.arg(m_document->pageCount()));
    };

    auto updatePageCount= [this]()
    {
        m_ui->m_curentPage->setMinimum(1);
        m_ui->m_curentPage->setMaximum(m_document->pageCount());
        m_ui->m_pageCount->setText(QString("/ %1").arg(m_document->pageCount()));
    };

    connect(nav, &QPdfPageNavigator::currentPageChanged, this, updateCurrentPage);
    connect(m_document.get(), &QPdfDocument::pageCountChanged, this, updatePageCount);
    connect(nav, &QPdfPageNavigator::forwardAvailableChanged, m_ui->m_nextPageAct, &QAction::setEnabled);
    connect(nav, &QPdfPageNavigator::backAvailableChanged, m_ui->m_previousPageAct, &QAction::setEnabled);

    m_ui->m_nextPageAct->setEnabled(nav->forwardAvailable());
    m_ui->m_previousPageAct->setEnabled(nav->backAvailable());

    updateCurrentPage();
    updatePageCount();

    connect(m_ui->m_nextPageAct, &QAction::triggered, nav, &QPdfPageNavigator::forward);
    connect(m_ui->m_previousPageAct, &QAction::triggered, nav, &QPdfPageNavigator::back);
    connect(m_ui->m_curentPage, &QSpinBox::valueChanged, this,
            [this, nav]() {
                nav->update(m_ui->m_curentPage->value() - 1, QPointF{0, 0}, nav->currentZoom());
            });

    m_ui->m_view->installEventFilter(this);

    connect(m_ui->m_exportToImage, &QAction::triggered, this, &PdfViewer::extractImage);
    connect(m_ui->m_exportToMapAct, &QAction::triggered, this, &PdfViewer::extractMap);
    connect(m_ui->m_extractTextAct, &QAction::triggered, this, &PdfViewer::extractText);

    connect(m_ui->m_cropViewAct, &QAction::triggered, this,
            [this]()
            {
                m_ui->m_exportToImage->setEnabled(m_ui->m_cropViewAct->isChecked());
                m_ui->m_exportToMapAct->setEnabled(m_ui->m_cropViewAct->isChecked());
                m_ui->m_extractTextAct->setEnabled(m_ui->m_cropViewAct->isChecked());
            });
    connect(m_ui->m_cropViewAct, &QAction::triggered, this, &PdfViewer::showOverLay);
}

bool PdfViewer::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::Resize && m_overlay && obj == m_ui->m_view)
    {
        auto geo= m_ui->m_view->geometry();
        m_overlay->setGeometry({m_ui->m_view->mapFromParent(geo.topLeft()), geo.size()});
        return QObject::eventFilter(obj, event);
    }
    else
    {
        return QObject::eventFilter(obj, event);
    }
}

void PdfViewer::extractImage()
{
    if(!m_overlay)
        return;

    auto rect= m_overlay->selectedRect();
    auto pix= m_ui->m_view->grab(rect);
    // m_pdfCtrl->shareImageIntoImage(pix);
    m_overlay.reset();
}

void PdfViewer::extractMap()
{
    if(!m_overlay)
        return;
    auto rect= m_overlay->selectedRect();
    m_ui->m_cropViewAct->setChecked(false);
    m_overlay.reset(nullptr);
    m_pdfCtrl->copyImage(m_ui->m_view->grab(rect));
    m_ui->m_exportToMapAct->setEnabled(false);
}
void PdfViewer::extractText()
{
    if(!m_overlay)
        return;
    /*auto rect= m_overlay->selectedRect();
    auto geo= m_ui->m_view->geometry();
    auto margins= m_ui->m_view->documentMargins();
    auto nav= m_ui->m_view->pageNavigator();
    auto pos= nav->currentLocation();
    auto page= nav->currentPage();
    auto zoomLevel= m_ui->m_view->zoomFactor();
    // auto pix= m_ui->m_view->select(rect);
    // m_pdfCtrl->shareImageIntoMap(pix);

    auto geoPage= geo.marginsRemoved(margins);

    auto rectPage= rect.marginsRemoved(QMargins{margins.left(), margins.top(), 0, 0});
    qCWarning(MediaContainerPDF) << "Geo:" << geo << "rect:" << rect << "margins:" << margins << "GeoPage:" << geoPage
                                 << "rectpage" << rectPage << "zoom" << zoomLevel;



    // qCWarning(MediaContainerPDF) << geoPage << rect << ;
    //  geo

    auto selections= m_document->getSelection(page, rectPage.toRectF().topLeft(), rectPage.toRectF().bottomRight());
    qCWarning(MediaContainerPDF) << selections.text();

    QRectF rectZoom(rect.x() * zoomLevel, rect.y() * zoomLevel, rect.width() * zoomLevel, rect.height() * zoomLevel);
    auto selections2= m_document->getSelection(page, rectZoom.topLeft(), rectZoom.bottomRight());
    qCWarning(MediaContainerPDF) << selections2.text();
    // m_overlay.reset();*/
}

void PdfViewer::sharePdfTo()
{
    auto answer
        = QMessageBox::question(this, tr("Sharing Pdf File"),
                                tr("PDF transfer can be really heavy.\nDo you want to continue and share the PDF?"),
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

    const int page= index.data(qToUnderlying<QPdfBookmarkModel::Role>(QPdfBookmarkModel::Role::Page)).toInt();
    m_ui->m_view->pageNavigator()->jump(page, {10, 10});
}

void PdfViewer::showOverLay()
{
    if(m_ui->m_cropViewAct->isChecked())
    {

        if(!m_overlay)
        {
            m_overlay.reset(new Overlay(m_ui->m_view));
        }
        auto geo= m_ui->m_view->geometry();
        m_overlay->setGeometry({m_ui->m_view->mapFromParent(geo.topLeft()), geo.size()});
        m_overlay->show();
    }
    else
    {
        if(m_overlay != nullptr)
        {
            m_overlay->hide();
            m_overlay.reset(nullptr);
            m_ui->m_exportToMapAct->setEnabled(false);
        }
    }
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

    menu.addAction(m_ui->m_cropViewAct);
    menu.addAction(m_ui->m_exportToMapAct);
    menu.addAction(m_ui->m_continuousAct);
    menu.addSeparator();
    menu.addAction(m_ui->m_zoomInAct);
    menu.addAction(m_ui->m_zoomOutAct);
    menu.addSeparator();
    menu.addAction(m_ui->m_shareAct);

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
