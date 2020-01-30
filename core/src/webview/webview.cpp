/***************************************************************************
 *     Copyright (C) 2018 by Renaud Guezennec                              *
 *     http://www.rolisteam.org/                                           *
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
#include <QVBoxLayout>

WebView::WebView(WebpageController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::WebViewContainer, parent)
    , m_webCtrl(ctrl)
    , m_ui(new Ui::WebView)
{
    auto wid= new QWidget();
    m_ui->setupUi(wid);
    setWidget(wid);

    setObjectName("WebPage");
    setGeometry(0, 0, 500, 500);
    setWindowIcon(QIcon(":/resources/icons/webPage.svg"));

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
    connect(m_ui->m_addressEdit, &QLineEdit::editingFinished, this,
            [this]() { m_webCtrl->setUrl(m_ui->m_addressEdit->text()); });
    connect(m_webCtrl, &WebpageController::urlChanged, this,
            [this]() { m_ui->m_webview->setUrl(QUrl::fromUserInput(m_webCtrl->url())); });

    connect(m_ui->m_webview, &QWebEngineView::loadFinished, this,
            [this]() { m_ui->m_webview->page()->toHtml([this](QString html) { m_webCtrl->setHtml(html); }); });

    connect(m_ui->m_reloadAct, &QAction::triggered, m_ui->m_webview, &QWebEngineView::reload);
    connect(m_ui->m_nextAct, &QAction::triggered, m_ui->m_webview, &QWebEngineView::forward);
    connect(m_ui->m_previousAct, &QAction::triggered, m_ui->m_webview, &QWebEngineView::back);
    connect(m_ui->m_hideAddressAct, &QAction::triggered, this, [this](bool b) {
        if(b)
            m_ui->m_addressEdit->setEchoMode(QLineEdit::Password);
        else
            m_ui->m_addressEdit->setEchoMode(QLineEdit::Normal);
    });
    m_ui->m_addressEdit->setFocusPolicy(Qt::StrongFocus);

    updateTitle();
    setWidget(wid);

    if(m_webCtrl->state() == WebpageController::LocalIsPlayer)
    {
        m_ui->m_htmlShareAct->setVisible(false);
        m_ui->m_shareAct->setVisible(false);
        m_ui->m_keepSharing->setVisible(false);
    }
}

WebView::~WebView() {}

bool WebView::readFileFromUri()
{
    /*if(nullptr == m_uri)
        return false;

    m_addressEdit->setText(m_uri->getUri());
    m_view->setUrl(QUrl(m_uri->getUri()));*/
    return true;
}

void WebView::saveMedia() {}

void WebView::putDataIntoCleverUri() {}

void WebView::updateTitle()
{
    setWindowTitle(tr("%1 - WebPage").arg(m_webCtrl->title()));
}

/*void WebView::sendOffClose()
{
    NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::closeMedia);
    msg.string8(m_mediaId);
    msg.sendToServer();
}*/

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

/*void WebView::fill(NetworkMessageWriter& message)
{
    message.string8(m_mediaId);
    // auto url= m_uri->getUri();
    // message.string16(getUriName());
    // message.string32(url);
}

void WebView::readMessage(NetworkMessageReader& msg)
{
   setUriName(msg.string16());
    auto typeMsg= static_cast<ShareType>(msg.uint8());
    auto url= msg.string32();
    qDebug() << "url: " << url << "name: " << getUriName() << typeMsg << "action:" << msg.action();
    if(typeMsg == HTML)
    {
        auto html= msg.string32();
        m_ui->m_webview->setHtml(html, QUrl::fromUserInput(url));
    }
    else if(typeMsg == URL)
    {
        m_ui->m_webview->setUrl(QUrl::fromUserInput(url));
    }
    // m_uri->setUri(url);
}*/

/*void WebView::createActions()
{
    if(m_webCtrl->state() == WebpageController::RemoteView)
        return;
*/
/*connect(m_shareAsLink, &QAction::triggered, this, [=](bool checked) {
     m_shareAsHtml->setEnabled(!checked);
     if(checked)
     {
         NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::addMedia);
         // msg.uint8(getContentType());
         msg.string8(m_mediaId);
         msg.string16(getUriName());
         msg.int8(URL);
         // msg.string32(m_uri->getUri());
         msg.sendToServer();
     }
     else
     {
         sendOffClose();
     }
 });*/

/*connect(m_shareAsHtml, &QAction::triggered, this, [=](bool checked) {
    m_shareAsLink->setEnabled(!checked);
    if(checked)
    {
        m_view->page()->toHtml([=](QString html) {
            NetworkMessageWriter msg(NetMsg::MediaCategory, NetMsg::addMedia);
            // msg.uint8(getContentType());
            msg.string8(m_mediaId);
            msg.string16(getUriName());
            msg.uint8(HTML);
            // msg.string32(m_uri->getUri());
            msg.string32(html);
            msg.sendToServer();
        });
    }
    else
    {
        sendOffClose();
    }
});*/
//}

/*void WebView::creationToolBar()
{
*/

/* connect(m_view, &QWebEngineView::urlChanged, this, [=]() {
     auto url= m_view->url().toString();
     // m_uri->setUri(url);
     if((m_shareAsHtml->isChecked() || m_shareAsLink->isChecked()) && m_webCtrl->keepSharing())
     {
         if(m_shareAsLink->isChecked())
         {
             NetworkMessageWriter msg(NetMsg::WebPageCategory, NetMsg::UpdateContent);
             msg.string8(m_mediaId);
             msg.string16(getUriName());
             msg.int8(URL);
             msg.string32(url);
             msg.sendToServer();
         }
         else if(m_shareAsHtml->isChecked())
         {
             m_view->page()->toHtml([=](QString html) {
                 NetworkMessageWriter msg(NetMsg::WebPageCategory, NetMsg::UpdateContent);
                 msg.string8(m_mediaId);
                 msg.string16(getUriName());
                 msg.uint8(HTML);
                 // msg.string32(m_uri->getUri());
                 msg.string32(html);
                 msg.sendToServer();
             });
         }
     }
 });*/
//}

void WebView::showEvent(QShowEvent* event)
{
    MediaContainer::showEvent(event);
    if(nullptr != m_ui->m_addressEdit)
        m_ui->m_addressEdit->setFocus();
}
