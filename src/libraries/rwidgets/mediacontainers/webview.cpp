/***************************************************************************
 *     Copyright (C) 2018 by Renaud Guezennec                              *
 *     https://rolisteam.org/                                           *
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
#include "webview.h"

#include "controller/view_controller/webpagecontroller.h"
#include "network/networkmessagereader.h"
#include "ui_webview.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

WebView::WebView(WebpageController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::WebViewContainer, parent)
    , m_webCtrl(ctrl)
    , m_ui(new Ui::WebView)
{
    auto wid= new QWidget();
    m_ui->setupUi(wid);

    setObjectName("WebPage");
    setGeometry(0, 0, 500, 500);
    setWindowIcon(QIcon::fromTheme("webPage"));

    m_ui->m_nextAct->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    m_ui->m_previousAct->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    m_ui->m_reloadAct->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

    m_ui->m_nextBtn->setDefaultAction(m_ui->m_nextAct);
    m_ui->m_previousBtn->setDefaultAction(m_ui->m_previousAct);
    m_ui->m_reloadBtn->setDefaultAction(m_ui->m_reloadAct);
    m_ui->m_hideAddressBtn->setDefaultAction(m_ui->m_hideAddressAct);

    m_ui->m_shareBtn->setDefaultAction(m_ui->m_shareAct);
    m_ui->m_htmlShareBtn->setDefaultAction(m_ui->m_htmlShareAct);

    connect(m_ui->m_keepSharing, &QCheckBox::clicked, m_webCtrl, &WebpageController::setKeepSharing);
    connect(m_webCtrl, &WebpageController::nameChanged, this, &WebView::updateTitle);
    connect(m_ui->m_webview, &QWebEngineView::titleChanged, m_webCtrl, &WebpageController::setName);
    connect(m_ui->m_webview, &QWebEngineView::urlChanged, m_webCtrl, &WebpageController::setPageUrl);
    connect(m_ui->m_addressEdit, &QLineEdit::editingFinished, this,
            [this]() { m_webCtrl->setPageUrl(QUrl::fromUserInput(m_ui->m_addressEdit->text())); });
    connect(m_webCtrl, &WebpageController::pageUrlChanged, this,
            [this]() { m_ui->m_webview->setUrl(m_webCtrl->pageUrl()); });

    connect(m_ui->m_webview, &QWebEngineView::loadFinished, this,
            [this]()
            {
                m_ui->m_webview->page()->toHtml(
                    [this](QString html)
                    {
                        if(m_webCtrl)
                            m_webCtrl->setHtml(html);
                    });
            });

    connect(m_ui->m_reloadAct, &QAction::triggered, m_ui->m_webview, &QWebEngineView::reload);
    connect(m_ui->m_nextAct, &QAction::triggered, m_ui->m_webview, &QWebEngineView::forward);
    connect(m_ui->m_previousAct, &QAction::triggered, m_ui->m_webview, &QWebEngineView::back);
    connect(m_ui->m_hideAddressAct, &QAction::triggered, this,
            [this](bool b) { m_ui->m_addressEdit->setEchoMode(b ? QLineEdit::Password : QLineEdit::Normal); });
    m_ui->m_addressEdit->setFocusPolicy(Qt::StrongFocus);

    connect(m_ui->m_shareAct, &QAction::triggered, m_webCtrl, &WebpageController::setUrlSharing);
    connect(m_ui->m_htmlShareAct, &QAction::triggered, m_webCtrl, &WebpageController::setHtmlSharing);
    connect(m_webCtrl, &WebpageController::sharingModeChanged,
            [this]()
            {
                m_ui->m_shareAct->setChecked(m_webCtrl->urlSharing());
                m_ui->m_htmlShareAct->setChecked(m_webCtrl->htmlSharing());
            });

    updateTitle();
    setWidget(wid);

    if(m_webCtrl)
    {
        if(m_webCtrl->state() != WebpageController::localIsGM)
        {
            m_ui->m_shareBtn->setVisible(false);
            m_ui->m_htmlShareBtn->setVisible(false);
            m_ui->m_htmlShareAct->setVisible(false);
            m_ui->m_shareAct->setVisible(false);
            m_ui->m_keepSharing->setVisible(false);
        }
        if(m_webCtrl->state() == WebpageController::RemoteView)
        {
            m_ui->m_nextAct->setVisible(false);
            m_ui->m_nextBtn->setVisible(false);
            m_ui->m_previousBtn->setVisible(false);
            m_ui->m_previousAct->setVisible(false);
            m_ui->m_addressEdit->setReadOnly(true);
        }

        m_ui->m_webview->setHtml(m_webCtrl->html());
        m_ui->m_webview->setUrl(m_webCtrl->pageUrl());
        m_ui->m_addressEdit->setText(m_webCtrl->pageUrl().toString());
        m_ui->m_hideAddressAct->setChecked(m_webCtrl->hideUrl());
        m_ui->m_addressEdit->setEchoMode(m_webCtrl->hideUrl() ? QLineEdit::Password : QLineEdit::Normal);

        qDebug() << "webview " << m_webCtrl->pageUrl() << m_webCtrl->html();
    }
}

WebView::~WebView() {}

void WebView::updateTitle()
{
    if(m_webCtrl)
        setWindowTitle(tr("%1 - WebPage").arg(m_webCtrl->title()));
}

void WebView::mousePressEvent(QMouseEvent* mouseEvent)
{
    if(mouseEvent->button() == Qt::BackButton)
    {
        m_ui->m_webview->back();
    }
    else if(mouseEvent->button() == Qt::ForwardButton)
    {
        m_ui->m_webview->forward();
    }
    MediaContainer::mousePressEvent(mouseEvent);
}

void WebView::showEvent(QShowEvent* event)
{
    MediaContainer::showEvent(event);
    if(nullptr != m_ui->m_addressEdit)
        m_ui->m_addressEdit->setFocus();
}
