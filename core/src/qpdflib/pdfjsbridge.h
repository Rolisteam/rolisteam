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

#ifndef PDFJSBRIDGE_H
#define PDFJSBRIDGE_H

#include <QPointer>
#include <QSemaphore>
#include <QWebEngineView>

class BridgeObject;

class PdfJsBridge : public QWebEngineView
{
    Q_OBJECT

    friend class BridgeObject;

public:
    explicit PdfJsBridge(QWidget* pParent= nullptr);
    ~PdfJsBridge();

    void invokeJavaScript(const QString& script);

    QVariant invokeJavaScriptAndWaitForResult(const QString& script);

    QStringList fetchPdfDocumentDestinations();

    void setToolbarVisible(bool visible);

    void close();

signals:

    void pdfDocumentloaded();

protected:
    void contextMenuEvent(QContextMenuEvent* pEvent);

private slots:

    void onLoadFinished(bool ok);

private:
    void jsInitialized();
    void jsReportDestinations(const QStringList& destinations);
    void jsLoaded();
    void jsClosed();

    void establishWebChannel();

    QWebChannel* m_pWebChannel;
    QSemaphore m_pdfDestinationsSema;

    QStringList m_pdfDocumentDestinations;

    QSemaphore m_pdfCloseSema;

    BridgeObject* m_pBridgeObject;
};

class BridgeObject : public QObject
{
    Q_OBJECT
public:
    explicit BridgeObject(PdfJsBridge* pBridge);

public slots:

    void jsInitialized();
    void jsReportDestinations(const QStringList& destinations);
    void jsLoaded();
    void jsClosed();

private:
    QPointer<PdfJsBridge> m_bridgePtr;
};

#endif // PDFJSBRIDGE_H
