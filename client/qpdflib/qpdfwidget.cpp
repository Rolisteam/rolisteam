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

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QVBoxLayout>
#include "pdfjsbridge.h"
#include "qpdfwidget.h"

// NOTE:
// There is this bug in Qt https://bugreports.qt.io/browse/QTBUG-46973
// which causes the application to crash when loading from qrc:/ resources.
// Reported to be fixed as from Qt 5.6.0

const QString cPdfViewerUrl("qrc:/resources/js/pdfview/viewer.html");

// class private members
struct QPdfWidgetPrivate
{
    PdfJsBridge *pPdfJsBridge;
    bool ready;
    QByteArray pdfData;
    QString pdfFile;
};

QPdfWidget::QPdfWidget(QWidget *pParent)
    : QWidget(pParent)
{
    m = new QPdfWidgetPrivate;
    m->pPdfJsBridge = new PdfJsBridge(this);
    connect(m->pPdfJsBridge, &PdfJsBridge::loadFinished, this, &QPdfWidget::onLoadFinished);
    connect(m->pPdfJsBridge, &PdfJsBridge::pdfDocumentloaded, this, &QPdfWidget::onRenderPdfFinished);
    m->ready = false;

    // Initialize pdf.js viewer
    QUrl url = QUrl::fromUserInput(cPdfViewerUrl);
    m->pPdfJsBridge->setUrl(url);

    // widget layout
    QVBoxLayout *pLayout = new QVBoxLayout();
    pLayout->setMargin(0);
    pLayout->addWidget(m->pPdfJsBridge);
    setLayout(pLayout);
}

QPdfWidget::~QPdfWidget()
{
    delete m;
}

bool QPdfWidget::loadFile(const QString &path)
{
    QFileInfo fi(path);
    if (fi.exists()) {
        m->pdfFile = path;
        renderPdfFile(path);
        return true;
    }

    return false;
}

bool QPdfWidget::loadData(const QByteArray &data)
{
    if(data.isEmpty())
        return false;

    m->pdfData = data;
    renderPdfData();
    return true;
}

void QPdfWidget::close()
{
    m->pPdfJsBridge->invokeJavaScript("PDFViewerApplication.close()");
}

void QPdfWidget::setPage(int page)
{
    QString script = QString("PDFViewerApplication.page = %1")
                        .arg(page);
    m->pPdfJsBridge->invokeJavaScript(script);
}

int QPdfWidget::page() const
{
    QVariant res = m->pPdfJsBridge->invokeJavaScriptAndWaitForResult("PDFViewerApplication.page");
    if (res.isValid()) {
        return res.toInt();
    }
    return 0;
}

int QPdfWidget::pagesCount() const
{
    QVariant res = m->pPdfJsBridge->invokeJavaScriptAndWaitForResult("PDFViewerApplication.pagesCount");
    if (res.isValid()) {
        return res.toInt();
    }
    return 0;
}

void QPdfWidget::rotatePages(int degrees)
{
    QString script = QString("PDFViewerApplication.rotatePages(%1)")
                        .arg(degrees);
    m->pPdfJsBridge->invokeJavaScript(script);
}

void QPdfWidget::zoomIn(int ticks)
{
    QString script = QString("PDFViewerApplication.zoomIn(%1)").arg(ticks);
    m->pPdfJsBridge->invokeJavaScript(script);
}

void QPdfWidget::zoomOut(int ticks)
{
    QString script = QString("PDFViewerApplication.zoomOut(%1)").arg(ticks);
    m->pPdfJsBridge->invokeJavaScript(script);
}

void QPdfWidget::zoomReset(qreal scale)
{
    QString script = QString("PDFViewerApplication.pdfViewer.currentScaleValue=%1").arg(scale);
    m->pPdfJsBridge->invokeJavaScript(script);
}

void QPdfWidget::showDocumentProperties()
{
    m->pPdfJsBridge->invokeJavaScript("PDFViewerApplication.pdfDocumentProperties.open()");
}

void QPdfWidget::setToolbarVisible(bool v)
{
    m->pPdfJsBridge->setToolbarVisible(v);
}

void QPdfWidget::setFindBarVisible(bool v)
{
    QString script = QString("PDFViewerApplication.findBar.%1()")
            .arg(v ? "open" : "close");
    m->pPdfJsBridge->invokeJavaScript(script);
}

void QPdfWidget::findText(const QString &text)
{
    if (!text.isEmpty()) {
        setFindFieldText(text);
    }
    // Sending an empty event will actually send 'find' since it will be prepended to
    // the event name.
    // All options options are (viewer.js:1155):
    // 'find',
    // 'findagain',
    // 'findhighlightallchange',
    // 'findcasesensitivitychange'
    m->pPdfJsBridge->invokeJavaScript("PDFViewerApplication.findBar.dispatchEvent('', false);");
}

void QPdfWidget::findNext()
{
    m->pPdfJsBridge->invokeJavaScript("PDFViewerApplication.findBar.dispatchEvent('again', false);");
}

void QPdfWidget::findPrevious()
{
    m->pPdfJsBridge->invokeJavaScript("PDFViewerApplication.findBar.dispatchEvent('again', true);");
}

int QPdfWidget::findResultsCount() const
{
    QVariant res = m->pPdfJsBridge->invokeJavaScriptAndWaitForResult("PDFViewerApplication.findController.matchCount");
    if (res.isValid()) {
        return res.toInt();
    }
    return 0;
}

void QPdfWidget::navigateTo(const QString &dest)
{
    QString escapedDest = dest;
    escapedDest.replace("\"", "\\\"");
    QString script = QString("PDFViewerApplication.pdfLinkService.navigateTo(\"%1\")")
            .arg(escapedDest);
    m->pPdfJsBridge->invokeJavaScript(script);
}

QStringList QPdfWidget::getDestinations() const
{
    return m->pPdfJsBridge->fetchPdfDocumentDestinations();
}

QWebEngineView* QPdfWidget::internalWebEngineView() const
{
    return dynamic_cast<QWebEngineView*>(m->pPdfJsBridge);
}

void QPdfWidget::onLoadFinished(bool status)
{
    if (!status) {
        qCritical() << "Unable to initialize the web view";
        return;
    }

    m->ready = true;
    if (!m->pdfData.isEmpty()) {
        renderPdfData();
    } else if (!m->pdfFile.isEmpty()) {
        renderPdfFile(m->pdfFile);
    }
}

void QPdfWidget::renderPdfData()
{
    if (!m->ready) {
        return;
    }

    m->pdfData = m->pdfData.toBase64();
    QString script = QString("qpdf_ShowPdfFileBase64('%1')")
                        .arg(QString::fromUtf8(m->pdfData));

    m->pPdfJsBridge->invokeJavaScript(script);

    // Clear pdf data
    m->pdfData.clear();
}

void QPdfWidget::renderPdfFile(const QString &file)
{
    if (!m->ready) {
        return;
    }

    m->pdfData = m->pdfData.toBase64();
    QString script = QString("qpdf_ShowPdfFile('file://%1')")
                        .arg(file);

    m->pPdfJsBridge->invokeJavaScript(script);

    // Clear pdf data
    m->pdfData.clear();
}


void QPdfWidget::onRenderPdfFinished()
{
    emit pdfDocumentLoaded();
}

void QPdfWidget::setFindFieldText(const QString &text)
{
    // Reset find controller.
    m->pPdfJsBridge->invokeJavaScriptAndWaitForResult("PDFViewerApplication.findController.reset()");

    // Fill-in the find field.
    QString escapedText = text;
    escapedText.replace("\"", "\\\"");
    QString script = QString("PDFViewerApplication.findBar.findField.value=\"%1\"")
            .arg(escapedText);
    m->pPdfJsBridge->invokeJavaScript(script);
}
