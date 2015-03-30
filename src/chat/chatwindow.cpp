/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/


#include "chatwindow.h"

#include <QDateTime>
#include <QScrollBar>
#include <QTextStream>
#include <QTimer>
#include <QToolBar>
#include <QVBoxLayout>
#include <QDir>
#include <QPushButton>

#include "chat/chat.h"
#include "network/networkmessagewriter.h"
#include "initialisation.h"
#include "localpersonmodel.h"
#include "mainwindow.h"
#include "data/persons.h"
#include "userlist/playersList.h"
#include "chat/improvedtextedit.h"

#include "types.h"

QStringList ChatWindow::m_keyWordList;

ChatWindow::ChatWindow(AbstractChat * chat, MainWindow * parent)
    : QWidget(), m_chat(chat), m_filename("%1/%2.html"),m_mainWindow(parent)/*,ui(new Ui::ChatWindow)*/
{
    m_preferences = PreferencesManager::getInstance();
    if (m_chat == NULL)
    {
        qFatal("ChatWindow with NULL chat");
    }
    // Initialisation des variables
    m_warnedEmoteUnavailable = false;
    m_hasUnseenMessage = false;

    // static members
    if (m_keyWordList.size() == 0)
    {
        m_keyWordList << "e " << "em " << "me " << "emote ";
    }
    setupUi();
    connect(m_editionZone, SIGNAL(textValidated(QString,QString)), this, SLOT(emettreTexte(QString,QString)));
    connect(m_editionZone, SIGNAL(ctrlUp()), this, SLOT(upSelectPerson()));
    connect(m_editionZone, SIGNAL(ctrlDown()), this, SLOT(downSelectPerson()));
    connect(m_mainWindow, SIGNAL(closing()), this, SLOT(save()));
    connect(m_chat, SIGNAL(changedMembers()), this, SLOT(scheduleUpdateChatMembers()));

    m_toggleViewAction = new QAction(this);
    m_toggleViewAction->setCheckable(true);

    m_diceParser = new DiceParser();
}
QMdiSubWindow* ChatWindow::getSubWindow()
{
    return m_window;
}

void ChatWindow::setupUi()
{
    // create and connect toggleViewAction
    m_splitter = new QSplitter();
    m_splitter->setOrientation(Qt::Vertical);
    //ChatRoom
    QVBoxLayout * vboxLayout = new QVBoxLayout();
    vboxLayout->setMargin(0);
    vboxLayout->setSpacing(0);

    vboxLayout->addWidget(m_splitter);
    m_displayZone= new ChatBrowser();
    m_displayZone->setOpenExternalLinks(true);
    m_displayZone->setReadOnly(true);
    m_displayZone->setMinimumHeight(30);

    m_editionZone = new ImprovedTextEdit();
    m_editionZone->setReadOnly(false);
    m_editionZone->setMinimumHeight(30);
    m_editionZone->setAcceptRichText(false);
    m_editionZone->installEventFilter(this);

    connect(m_editionZone,SIGNAL(receivedFocus()),this,SLOT(editionGetFocus()));

    // Layout
    m_bottomWidget = new QWidget();
    m_selectPersonComboBox = new QComboBox();
    m_selectPersonComboBox->setModel(&LocalPersonModel::instance());
    m_selectPersonComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    // Toolbar
    QToolBar * toolBar = new QToolBar();
    //toolBar->setOrientation(Qt::Horizontal);
    toolBar->addWidget(m_selectPersonComboBox);
    toolBar->addAction(QIcon::fromTheme("document-save", QIcon(":/resources/icons/save.png")),tr("save"), this, SLOT(save()));

    // SelectPersonComboBox
    scheduleUpdateChatMembers();

    QVBoxLayout * internalVLayout = new QVBoxLayout();
    internalVLayout->setMargin(0);
    internalVLayout->setSpacing(0);

    internalVLayout->addWidget(toolBar);
    internalVLayout->addWidget(m_editionZone);
//zoneEdition
    m_bottomWidget->setLayout(internalVLayout);


    m_splitter->addWidget(m_displayZone);
    m_splitter->addWidget(m_bottomWidget);


    // Initialisation des tailles des 2 widgets
    QList<int> tailles;
    tailles.append(200);
    tailles.append(40);
    m_splitter->setSizes(tailles);

    // Window initialisation
    setObjectName("ChatWindow");
    setAttribute(Qt::WA_DeleteOnClose, false);
    setLayout(vboxLayout);

}

/********************************************************************/    
/* Destructeur                                                      */
/********************************************************************/    
ChatWindow::~ChatWindow()
{
    delete m_toggleViewAction;

    if (PreferencesManager::getInstance()->value("isClient",true).toBool() || !m_chat->inherits("PrivateChat"))
        delete m_chat;
}


AbstractChat * ChatWindow::chat() const
{
    return m_chat;
}
bool ChatWindow::isVisible()
{
    return (m_window->isVisible() & QWidget::isVisible());
}

// not (const QString & message), because we change it !
void ChatWindow::emettreTexte(QString messagehtml,QString message)
{
    //NetMsg::ChatMessageAction, NetMsg::DiceMessageAction, NetMsg::EmoteMessageAction
    NetMsg::Action action = NetMsg::DiceMessageAction;

    bool ok=true;
    QString tirage;
    int result;
    m_editionZone->clear();


    QString localPersonIdentifier = m_selectPersonComboBox->itemData(m_selectPersonComboBox->currentIndex(), PlayersList::IdentifierRole).toString();
    Person * localPerson = PlayersList::instance()->getPerson(localPersonIdentifier);

    QString tmpmessage=message.simplified();
    QString messageCorps="";
    QString messageTitle="";
    QColor color;

    switch(tmpmessage[0].toLatin1())
    {
    case '!':
        tmpmessage=tmpmessage.remove(0,1);
        if(m_diceParser->parseLine(tmpmessage))
        {
            m_diceParser->Start();
            messageCorps = m_diceParser->displayResult();
            messageTitle = tr("You");
            color = localPerson->color();
            showMessage(messageTitle, color, messageCorps,NetMsg::DiceMessageAction);
            message = messageCorps;
        }
        else
        {
            messageCorps = tr("!1d6 or !5d10+3 or !2d20-3d10+1d6+5 etc... The dice roll is public (For private roll, use & ).");
            messageTitle = tr("Syntax");
            color = Qt::red;
            showMessage(messageTitle, color, messageCorps);
        }
        break;
    case '&':
        if (ok)
        {
            messageCorps = tr("You got %1 at your secret dice roll [%2]").arg(result).arg(tirage);
            messageTitle = tr("Secret Roll:");
            color = Qt::magenta;
            showMessage(messageTitle, color,messageCorps ,NetMsg::DiceMessageAction);
        }
        else
        {
            messageCorps = tr("!1d6 or !5d10+3 or !2d20-3d10+1d6+5 etc... The dice roll is public (For private roll, use & ).");
            messageTitle = tr("Syntax");
            color = Qt::red;
            showMessage(messageTitle, color, messageCorps);
        }
        return;
        break;
    case '/':
    {
        bool emote = false;

        for(int i = 0;((i < m_keyWordList.size())&&(!emote)); i++)
        {
            QString s = m_keyWordList[i];

            if(tmpmessage.indexOf(s) == 1)
            {
                emote = true;

                message.remove(0,s.length()+1);
            }

        }
        if(emote)
        {
            // Warn if some users don't have Emote(0) feature
            if (!m_warnedEmoteUnavailable && !m_chat->everyPlayerHasFeature(QString("Emote")))
            {
                messageTitle = tr("Warning");
                messageCorps = tr("Some users won't be enable to see your emotes.");
                color = Qt::red;
                showMessage(messageTitle, color, messageCorps);
                m_warnedEmoteUnavailable = true;
            }


            showMessage(localPerson->name(), localPerson->color(), message,NetMsg::EmoteMessageAction);
            action = NetMsg::EmoteMessageAction;
            break;
        }
    }

    default:
        messageTitle = localPerson->name();
        /* if(m_chat->everyPlayerHasFeature(QString("RichTextChat")))
            {
                message=messagehtml;
            }*/
        showMessage(messageTitle, localPerson->color(), message);
        // action is messageChatWindow only if there are no dices
        action = NetMsg::ChatMessageAction;
        break;
    }

    if(!ok)
        return;

    // Emission du message
    NetworkMessageWriter data(NetMsg::ChatCategory, action);
    data.string8(localPersonIdentifier);
    data.string8(m_chat->identifier());
    data.string32(message);
    m_chat->sendThem(data);
}


QAction * ChatWindow::toggleViewAction() const
{
    return m_toggleViewAction;
}


void ChatWindow::showMessage(const QString& utilisateur, const QColor& couleur, const QString& message, NetMsg::Action msgtype)
{
    m_displayZone->moveCursor(QTextCursor::End);
    m_displayZone->setTextColor(couleur);
    QString msg = message;
    switch(msgtype)
    {
    case NetMsg::ChatMessageAction:
        m_displayZone->setFontItalic(false);
        m_displayZone->append(QString("%1 : ").arg(utilisateur));
        break;
    case NetMsg::EmoteMessageAction:
        m_displayZone->setFontItalic(true);
        m_displayZone->append(QString("%1 ").arg(utilisateur));
        msg=QString("<i>%1</i>").arg(message);
        break;
    case NetMsg::DiceMessageAction:
        m_displayZone->setFontItalic(false);
        m_displayZone->append(QString("%1 ").arg(utilisateur));
        break;
    default:
        qWarning("Can't type unknown message type %d", msgtype);
        return;
    }

    if (!m_editionZone->hasFocus() && !m_hasUnseenMessage)
    {
        m_hasUnseenMessage = true;
        emit ChatWindowHasChanged(this);
    }

    // Affichage du message
    m_displayZone->setTextColor(Qt::black);


    m_displayZone->insertHtml(msg);
   // qDebug() << message;
    if(msgtype==NetMsg::EmoteMessageAction)
    {
        m_displayZone->setFontItalic(false);
    }
    // On repositionne la barre de defilement, pour pouvoir voir le texte qui vient d'etre affiche
    m_displayZone->verticalScrollBar()->setSliderPosition(m_displayZone->verticalScrollBar()->maximum());
}



bool ChatWindow::hasUnseenMessage() const
{
    return m_hasUnseenMessage;
}



void ChatWindow::save()
{
    QString filename = m_filename.arg(m_preferences->value("ChatDirectory",QDir::homePath()).toString(), m_chat->name());
    QFile file (filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning("Unable to open %s", qPrintable(filename));
        return;
    }

    QTextStream stream (&file);
    stream << m_displayZone->document()->toHtml(QByteArray("utf-8"));

    file.close();
}

QString ChatWindow::getTitleFromChat()
{
    m_toggleViewAction->setText(m_chat->name());
    return m_chat->name();
}


void ChatWindow::showEvent(QShowEvent *event)
{
    m_editionZone->setFocus(Qt::OtherFocusReason);
    if(m_selectPersonComboBox->currentText().isEmpty())
    {
        m_selectPersonComboBox->setCurrentIndex(0);
    }
    if(NULL!=m_toggleViewAction)
    {
        m_toggleViewAction->setChecked(true);
    }
    emit ChatWindowHasChanged(this);
    //QWidget::showEvent(event);
}
void ChatWindow::hideEvent(QHideEvent *event)
{
    if(NULL!=m_toggleViewAction)
    {
        m_toggleViewAction->setChecked(false);
    }
    emit ChatWindowHasChanged(this);
    //QWidget::hideEvent(event);
}

void ChatWindow::editionGetFocus()
{
    m_hasUnseenMessage = false;
    emit ChatWindowHasChanged(this);
}

void ChatWindow::upSelectPerson()
{
    int pos = m_selectPersonComboBox->currentIndex();
    if (pos == 0)
        pos = m_selectPersonComboBox->count();
    m_selectPersonComboBox->setCurrentIndex(pos - 1);
}

void ChatWindow::downSelectPerson()
{
    int pos = m_selectPersonComboBox->currentIndex() + 1;
    if (pos == m_selectPersonComboBox->count())
        pos = 0;
    m_selectPersonComboBox->setCurrentIndex(pos);
}

void ChatWindow::scheduleUpdateChatMembers()
{
    //qDebug("schedules");
    QTimer::singleShot(300, this, SLOT(updateChatMembers()));
}

void ChatWindow::updateChatMembers()
{
    //qDebug("doing it");
    bool enable = m_chat->everyPlayerHasFeature(QString("MultiChat"));
    if (!enable)
    {
        m_selectPersonComboBox->setCurrentIndex(0);
    }
    m_selectPersonComboBox->setEnabled(enable);
}
void ChatWindow::setSubWindow(QMdiSubWindow* subWindow)
{
    m_window = subWindow;
    connect(m_toggleViewAction, SIGNAL(triggered(bool)), m_window, SLOT(setVisible(bool)));
}

