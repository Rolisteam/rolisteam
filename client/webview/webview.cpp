#include "webview.h"

#include <QVBoxLayout>


WebView::WebView(QWidget *parent)
    : MediaContainer(parent)
{
    setObjectName("WebPage");
    setWindowIcon(QIcon(":/resources/icons/webPage.svg"));
    auto wid = new QWidget();
    m_mainLayout = new QVBoxLayout(wid);
    wid->setLayout(m_mainLayout);

    m_view = new QWebEngineView();

    createActions();
    creationToolBar();

    m_mainLayout->addWidget(m_view);

    updateTitle();

    setWidget(wid);
}

WebView::~WebView()
{

}

bool WebView::readFileFromUri()
{
    if(nullptr == m_uri)
        return false;

    m_view->setUrl(QUrl(m_uri->getUri()));
    return true;
}

void WebView::saveMedia()
{

}

void WebView::putDataIntoCleverUri()
{

}

void WebView::updateTitle()
{
    if(nullptr == m_uri)
        return;
    setWindowTitle(tr("%1 - WebPage").arg(m_uri->name()));
}

void WebView::cleverURIHasChanged(CleverURI* uri, CleverURI::DataValue datav)
{
    if(uri != m_uri)
        return;

    if(datav == CleverURI::NAME)
    {
        updateTitle();
    }
}

void WebView::mousePressEvent(QMouseEvent* mouseEvent)
{
    if(mouseEvent->button() ==  Qt::BackButton)
    {
        m_view->back();
    }
    else if(mouseEvent->button() == Qt::ForwardButton)
    {
        m_view->forward();
    }
    MediaContainer::mousePressEvent(mouseEvent);

}

void WebView::createActions()
{
    m_shareAsLink = new QAction(tr("Share"),this);

   /* connect(m_shareAsLink,&QAction::triggered,[=](){
        NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::addMedia);
        msg.string32(m_uri->getUri());
    });*/

    m_shareAsHtml = new QAction(tr("Share Html"),this);
    //m_shareAsView = new QAction(tr("Share View"),this);
    m_next= new QAction(tr("next"),this);
    m_next->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    m_previous= new QAction(tr("Previous"),this);
    m_previous->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    m_reload= new QAction(tr("Reload"),this);
    m_reload->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

}

void WebView::fill(NetworkMessageWriter & message)
{
    message.string8(m_mediaId);
    auto url = m_uri->getUri();
    message.string32(url);
    message.string16(m_title);
}

void WebView::creationToolBar()
{
    auto hLayout = new QHBoxLayout(this);

    auto button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_previous);
    hLayout->addWidget(button);

    button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_next);
    hLayout->addWidget(button);

    button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_reload);
    hLayout->addWidget(button);

    m_addressEdit = new QLineEdit(this);
    hLayout->addWidget(m_addressEdit,1);

    button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_shareAsLink);
    hLayout->addWidget(button);


    button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_shareAsHtml);
    hLayout->addWidget(button);


  /*  button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_shareAsView);
    hLayout->addWidget(button);*/

    m_mainLayout->addLayout(hLayout);


    connect(m_addressEdit,&QLineEdit::editingFinished,[=](){
        m_view->setUrl(QUrl(m_addressEdit->text()));
    });
    connect(m_reload,&QAction::triggered,m_view,&QWebEngineView::reload);
    connect(m_next,&QAction::triggered,m_view,&QWebEngineView::forward);
    connect(m_previous,&QAction::triggered,m_view,&QWebEngineView::back);
    connect(m_view,&QWebEngineView::titleChanged,[=](){
        m_uri->setName(m_view->title());
    });
    connect(m_view,&QWebEngineView::urlChanged,[=](){
        auto url = m_view->url().toString();
        m_uri->setUri(url);
    });
   // connect(m_addressEdit,&QLineEdit::editingFinished,m_view,&QWebEngineView::setUrl);


}
