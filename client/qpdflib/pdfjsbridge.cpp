/*
                          qpdf

    Copyright (C) 2015 Arthur Benilov,
    arthur.benilov@gmail.com
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Lesser General Public License for more details.
*/

#include <QApplication>
#include <QWebChannel>
#include <QContextMenuEvent>
#include <QWebEngineSettings>
#include <QSemaphore>
#include "pdfjsbridge.h"

PdfJsBridge::PdfJsBridge(QWidget *pParent)
    : QWebEngineView(pParent)
{
    m_pWebChannel = new QWebChannel(this);
    m_pBridgeObject = new BridgeObject(this);

    connect(this, &QWebEngineView::loadFinished, this, &PdfJsBridge::onLoadFinished);
}

PdfJsBridge::~PdfJsBridge()
{
    delete m_pWebChannel;
}

void PdfJsBridge::invokeJavaScript(const QString &script)
{
    QWebEnginePage *pPage = page();
    if (pPage != nullptr) {
        pPage->runJavaScript(script);
    }
}

QVariant PdfJsBridge::invokeJavaScriptAndWaitForResult(const QString &script)
{
    QVariant result;
    QWebEnginePage *pPage = page();
    if (pPage != nullptr) {

        QSemaphore waitSemaphore;
        pPage->runJavaScript(script, [&result, &waitSemaphore](const QVariant &res) {
            result = res;
            waitSemaphore.release();
        });

        while (!waitSemaphore.tryAcquire()) {
            qApp->processEvents();
        }
    }

    return result;
}

QStringList PdfJsBridge::fetchPdfDocumentDestinations()
{
    m_pdfDocumentDestinations.clear();

    invokeJavaScript("qpdf_FetchDestinations();");

    while (!m_pdfDestinationsSema.tryAcquire()) {
        qApp->processEvents();
    }

    return m_pdfDocumentDestinations;
}

void PdfJsBridge::setToolbarVisible(bool visible)
{
    invokeJavaScript(QString("qpdf_ShowToolbar(%1)")
                     .arg(visible ? "true" : "false"));
}

void PdfJsBridge::close()
{
    invokeJavaScript("qpdf_Close();");
    while (!m_pdfCloseSema.tryAcquire()) {
        qApp->processEvents();
    }
}

void PdfJsBridge::jsInitialized()
{
    // Web view has been initialized
}

void PdfJsBridge::jsReportDestinations(const QStringList &destinations)
{
    m_pdfDocumentDestinations = destinations;
    m_pdfDestinationsSema.release();
}

void PdfJsBridge::jsLoaded()
{
    emit pdfDocumentloaded();
}

void PdfJsBridge::jsClosed()
{
    m_pdfCloseSema.release();
}

void PdfJsBridge::contextMenuEvent(QContextMenuEvent *pEvent)
{
    // Disable context menu completely
    pEvent->ignore();
}

void PdfJsBridge::onLoadFinished(bool ok)
{
    if (ok) {
        establishWebChannel();
    }
}

void PdfJsBridge::establishWebChannel()
{
    QWebEnginePage *pPage = page();
    if (pPage != nullptr) {
        pPage->setWebChannel(m_pWebChannel);
        m_pWebChannel->registerObject(QStringLiteral("qpdfbridge"), m_pBridgeObject);

        pPage->runJavaScript("qpdf_Initialize();");
    }
}

//
//  BridgeObject implementation
//

BridgeObject::BridgeObject(PdfJsBridge *pBridge)
    : QObject(pBridge),
      m_bridgePtr(pBridge)
{
    Q_ASSERT(pBridge != nullptr);
}

void BridgeObject::jsInitialized()
{
    m_bridgePtr->jsInitialized();
}

void BridgeObject::jsReportDestinations(const QStringList &destinations)
{
    m_bridgePtr->jsReportDestinations(destinations);
}

void BridgeObject::jsLoaded()
{
    m_bridgePtr->jsLoaded();
}

void BridgeObject::jsClosed()
{
    m_bridgePtr->jsClosed();
}
