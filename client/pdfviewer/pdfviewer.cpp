/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://www.rolisteam.org/contact                   *
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
#include <QDebug>
#include <QFileDialog>
#include <QScrollBar>
#include <QShortcut>
#include <QtGui>

#include "widgets/overlay.h"

#include "qpdflib/qpdfwidget.h"

#include "mainwindow.h"
#include "network/networkmessagewriter.h"

#define MARGING 0
#define ICON_SIZE 32
/********************************************************************/
/* Constructeur                                                     */
/********************************************************************/
PdfViewer::PdfViewer(/*QString title,QString identPdfViewer, QString identJoueur, QPdfViewer *image, QAction *action,*/
                     ImprovedWorkspace* parent)
    : MediaContainer(MediaContainer::ContainerType::PDFContainer, parent)
{
    setObjectName("PdfViewer");
    setWindowIcon(QIcon(":/resources/icons/pdfLogo.png"));
    auto wid= new QWidget();
    m_mainLayout= new QVBoxLayout(wid);
    m_mainLayout->setMargin(MARGING);
    wid->setLayout(m_mainLayout);

    createActions();
    creationToolBar();

    m_pdfWidget= new QPdfWidget();
    m_mainLayout->addWidget(m_pdfWidget);

    setWidget(wid);
}

PdfViewer::~PdfViewer() {}

void PdfViewer::creationToolBar()
{
    // m_mainLayout
    auto hLayout= new QHBoxLayout(this);
    hLayout->setMargin(MARGING);

    auto button= new QToolButton(this);

    button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    button->setAutoRaise(true);
    m_cropCurrentView->setCheckable(true);
    button->setDefaultAction(m_cropCurrentView);
    hLayout->addWidget(button);

    button= new QToolButton(this);
    button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    button->setAutoRaise(true);
    button->setDefaultAction(m_shareDocument);
    hLayout->addWidget(button);

    button= new QToolButton(this);
    button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    button->setAutoRaise(true);
    button->setDefaultAction(m_toMap);
    m_cropOption.push_back(m_toMap);
    hLayout->addWidget(button);

    button= new QToolButton(this);
    button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    button->setAutoRaise(true);
    button->setDefaultAction(m_toVmap);
    m_cropOption.push_back(m_toVmap);
    hLayout->addWidget(button);

    button= new QToolButton(this);
    button->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    button->setAutoRaise(true);
    button->setDefaultAction(m_image);
    m_cropOption.push_back(m_image);
    hLayout->addWidget(button);

    hLayout->addStretch();

    m_mainLayout->addLayout(hLayout);
    for(auto act : m_cropOption)
    {
        act->setVisible(false);
    }
}

void PdfViewer::createActions()
{
    m_cropCurrentView= new QAction(tr("Crop Current View"), this);
    m_cropCurrentView->setIcon(QIcon(":/resources/icons/crop.svg"));

    connect(m_cropCurrentView, &QAction::triggered, this, &PdfViewer::showOverLay);

    m_shareDocument= new QAction(tr("Document to all"), this);
    m_shareDocument->setIcon(QIcon(":/resources/icons/document-share.svg"));

    m_toMap= new QAction(tr("Export to Map"), this);
    m_toMap->setData(CleverURI::MAP);

    m_toVmap= new QAction(tr("Export into VMap"), this);
    m_toVmap->setData(CleverURI::VMAP);

    m_image= new QAction(tr("Export as Image"), this);
    m_image->setData(CleverURI::PICTURE);

    connect(m_toMap, &QAction::triggered, this, &PdfViewer::exportImage);
    connect(m_toVmap, &QAction::triggered, this, &PdfViewer::exportImage);
    connect(m_image, &QAction::triggered, this, &PdfViewer::exportImage);
    connect(m_shareDocument, &QAction::triggered, this, &PdfViewer::sharePdfTo);
}

void PdfViewer::exportImage()
{
    if(nullptr != m_overlay)
    {
        auto act= qobject_cast<QAction*>(sender());
        auto type= static_cast<CleverURI::ContentType>(act->data().toInt());

        auto rect= m_overlay->rect();
        auto pix= m_pdfWidget->grab(rect);

        emit openImageAs(pix, type);

        delete m_overlay;
        m_overlay= nullptr;
        m_cropCurrentView->setChecked(false);
    }
}

void PdfViewer::sharePdfTo()
{
    auto answer
        = QMessageBox::question(this, tr("Sharing Pdf File"),
                                tr("PDF transfert can be really heavy.\nDo you want to continue and share the PDF?"),
                                QMessageBox::Yes | QMessageBox::Cancel);
    if(answer == QMessageBox::Yes)
    {
        NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::addMedia);
        msg.uint8(getContentType());
        fill(msg);
        msg.sendToServer();
    }
}

void PdfViewer::updateTitle()
{
    setWindowTitle(tr("%1 - (PDF)").arg(getUriName()));
}

void PdfViewer::showOverLay()
{
    if(m_cropCurrentView->isChecked())
    {
        if(m_overlay == nullptr)
        {
            m_overlay= new Overlay(m_pdfWidget->geometry(), m_pdfWidget->parentWidget());
        }
        m_overlay->setGeometry(m_pdfWidget->geometry());
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
    }
}
void PdfViewer::fill(NetworkMessageWriter& message)
{
    QByteArray baPdfViewer= m_uri->getData();
    if(baPdfViewer.isEmpty())
    {
        QFile pdfFile(m_uri->getUri());
        if(!pdfFile.exists())
            return;

        if(pdfFile.open(QIODevice::ReadOnly))
        {
            baPdfViewer= pdfFile.readAll();
        }
    }

    message.string16(getUriName());
    message.string8(m_mediaId);
    message.byteArray32(baPdfViewer);
}

void PdfViewer::readMessage(NetworkMessageReader& msg)
{
    setUriName(msg.string16());

    m_mediaId= msg.string8();
    QByteArray data= msg.byteArray32();
    // QTemporaryFile file;
    // file.setAutoRemove(false);
    // file.open();
    // file.write(data);
    m_pdfWidget->loadData(data);
    m_remote= true;
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

    menu.addAction(m_cropCurrentView);
    menu.addAction(m_shareDocument);
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

void PdfViewer::putDataIntoCleverUri()
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    savePdfToFile(out);
    if(nullptr != m_uri)
    {
        m_uri->setData(data);
    }
}
bool PdfViewer::readFileFromUri()
{
    /*if(!m_uri->hasData())
    {
        QByteArray array;
        m_uri->loadFileFromUri(array);
        m_uri->setData(array);
    }
    const auto& data = m_uri->getData();*/

    if(m_pdfWidget->loadFile(m_uri->getUri()))
    {
        updateTitle();
        return true;
    }
    return false;
}
bool PdfViewer::openMedia()
{
    return false;
}
void PdfViewer::saveMedia()
{
    /// nothing to be done.
}
