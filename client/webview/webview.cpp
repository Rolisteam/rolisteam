#include "webview.h"

#include <QVBoxLayout>
#include "network/networkmessagereader.h"

WebView::WebView(bool localIsOwner,QWidget *parent)
    : MediaContainer(localIsOwner,parent)
{
    setObjectName("WebPage");
    setWindowIcon(QIcon(":/resources/icons/webPage.svg"));
    m_uri = new CleverURI("Webpage","",CleverURI::WEBVIEW);
    auto wid = new QWidget();
    m_mainLayout = new QVBoxLayout(wid);



    m_view = new QWebEngineView();

    if(localIsOwner)
    {
        createActions();
        creationToolBar();
    }
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


    m_addressEdit->setText(m_uri->getUri());
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
    setWindowTitle(tr("%1 - WebPage").arg(getUriName()));
}

void WebView::sendOffClose()
{
    NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::closeMedia);
    msg.string8(m_mediaId);
    msg.sendToServer();
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

void WebView::fill(NetworkMessageWriter & message)
{
    message.string8(m_mediaId);
    auto url = m_uri->getUri();
    message.string32(url);
    message.string16(getUriName());
}

void WebView::readMessage(NetworkMessageReader& msg)
{
    auto typeMsg = static_cast<ShareType>(msg.uint8());
    setUriName(msg.string16());
    auto url = msg.string32();
    if(typeMsg == HTML)
    {
        auto html = msg.string32();
        m_view->setHtml(html,QUrl::fromUserInput(url));
    }
    else if(typeMsg == URL)
    {
        m_view->setUrl(QUrl::fromUserInput(url));
    }
    m_uri->setUri(url);

}

void WebView::createActions()
{
    m_shareAsLink = new QAction(tr("Share"),this);
    m_shareAsLink->setCheckable(true);

    connect(m_shareAsLink,&QAction::triggered,[=](bool checked){
        m_shareAsHtml->setEnabled(!checked);
        if(checked)
        {
            NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::addMedia);
            msg.uint8(getContentType());
            msg.string8(m_mediaId);
            msg.string16(getUriName());
            msg.int8(URL);
            msg.string32(m_uri->getUri());
            msg.sendToServer();
        }
        else
        {
            sendOffClose();
        }
    });

    m_shareAsHtml = new QAction(tr("Share html"),this);
    m_shareAsHtml->setCheckable(true);


    connect(m_shareAsHtml, &QAction::triggered,[=](bool checked){
        m_shareAsLink->setEnabled(!checked);
        if(checked)
        {
            m_view->page()->toHtml([=](QString html){
                NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::addMedia);
                msg.uint8(getContentType());
                msg.string8(m_mediaId);
                msg.string16(getUriName());
                msg.uint8(HTML);
                msg.string32(m_uri->getUri());
                msg.string32(html);
                msg.sendToServer();
            });
        }
        else
        {
           sendOffClose();
        }
    });

    m_next= new QAction(tr("next"),this);
    m_next->setIcon(style()->standardIcon(QStyle::SP_ArrowForward));
    m_previous= new QAction(tr("Previous"),this);
    m_previous->setIcon(style()->standardIcon(QStyle::SP_ArrowBack));
    m_reload= new QAction(tr("Reload"),this);
    m_reload->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

    m_hideAddress = new QAction(tr("Hide Address Bar"),this);
    m_hideAddress->setCheckable(true);
    m_hideAddress->setIcon(QIcon(":/resources/icons/mask.png"));

}


void WebView::creationToolBar()
{
    auto hLayout = new QHBoxLayout();

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
    button->setDefaultAction(m_hideAddress);
    hLayout->addWidget(button);
    hLayout->addStretch();

    button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_shareAsLink);
    hLayout->addWidget(button);

    button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_shareAsHtml);
    hLayout->addWidget(button);

    button = new QToolButton(this);
    button->setAutoRaise(true);
    button->setDefaultAction(m_shareAsView);
    hLayout->addWidget(button);

    auto checkbox = new QCheckBox(tr("Keep Sharing"),this);
    connect(checkbox,&QCheckBox::clicked,this,[checkbox,this](){
        m_keepSharing = checkbox->isChecked();
    });
    hLayout->addWidget(checkbox);


    m_mainLayout->addLayout(hLayout);


    connect(m_addressEdit,&QLineEdit::editingFinished,[=](){
        m_view->setUrl(QUrl::fromUserInput(m_addressEdit->text()));
    });
    connect(m_reload,&QAction::triggered,m_view,&QWebEngineView::reload);
    connect(m_next,&QAction::triggered,m_view,&QWebEngineView::forward);
    connect(m_previous,&QAction::triggered,m_view,&QWebEngineView::back);
    connect(m_view,&QWebEngineView::titleChanged,[=](){
        m_uri->setName(m_view->title());
    });
    connect(m_hideAddress,&QAction::triggered,this,[=](bool b){
        if(b)
            m_addressEdit->setEchoMode(QLineEdit::Password);
        else
            m_addressEdit->setEchoMode(QLineEdit::Normal);
    });
    connect(m_view,&QWebEngineView::urlChanged,[=](){
        auto url = m_view->url().toString();
        m_uri->setUri(url);
        if((m_shareAsHtml->isChecked() || m_shareAsLink->isChecked() ) && m_keepSharing)
        {
            if(m_shareAsLink->isChecked())
            {
                NetworkMessageWriter msg(NetMsg::WebPageCategory,NetMsg::UpdateContent);
                msg.string8(m_mediaId);
                msg.string16(getUriName());
                msg.int8(URL);
                msg.string32(url);
                msg.sendToServer();
            }
            else if(m_shareAsHtml->isChecked())
            {
                m_view->page()->toHtml([=](QString html){
                    NetworkMessageWriter msg(NetMsg::WebPageCategory,NetMsg::UpdateContent);
                    msg.string8(m_mediaId);
                    msg.string16(getUriName());
                    msg.uint8(HTML);
                    msg.string32(m_uri->getUri());
                    msg.string32(html);
                    msg.sendToServer();
                });
            }
        }
    });
    m_addressEdit->setFocusPolicy(Qt::StrongFocus);

}
void WebView::showEvent(QShowEvent* event)
{
    MediaContainer::showEvent(event);
    if(nullptr != m_addressEdit)
        m_addressEdit->setFocus();
}
