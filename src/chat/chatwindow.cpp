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
#include <QMdiArea>

#include "chat/chat.h"
#include "network/networkmessagewriter.h"
#include "localpersonmodel.h"
#include "mainwindow.h"
#include "data/persons.h"
#include "userlist/playersList.h"
#include "chat/improvedtextedit.h"

#include "types.h"

QStringList ChatWindow::m_keyWordList;
QList<DiceAlias*>* ChatWindow::m_receivedAlias = NULL;

ChatWindow::ChatWindow(AbstractChat * chat, MainWindow * parent)
    : QWidget(), m_chat(chat), m_filename("%1/%2.html"),m_mainWindow(parent)
{
    m_preferences = PreferencesManager::getInstance();
    if (m_chat == NULL)
    {
        qFatal("ChatWindow with NULL chat");
    }
    m_warnedEmoteUnavailable = false;
    m_hasUnseenMessage = false;

    // static members
    if (m_keyWordList.size() == 0)
    {
        m_keyWordList << "e" << "em" << "me" << "emote";
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
    m_diceParser->setPathToHelp(tr("<a href=\"http://wiki.rolisteam.org/index.php/Dice_Rolling\">http://wiki.rolisteam.org/index.php/Dice_Rolling</a>"));

    m_operatorMap = new QMap<QString,CHAT_OPERATOR>();
    m_operatorMap->insert("/",COMMAND);
    m_operatorMap->insert("!",DICEROLL);
    m_operatorMap->insert("&",SECRET_DICEROLL);

}
QMdiSubWindow* ChatWindow::getSubWindow()
{
    return m_window;
}
void ChatWindow::updateListAlias()
{
    QList<DiceAlias*>* list = m_diceParser->getAliases();
    list->clear();
    int size = m_preferences->value("DiceAliasNumber",0).toInt();
    for(int i = 0; i < size ; ++i)
    {
        QString cmd = m_preferences->value(QString("DiceAlias_%1_command").arg(i),"").toString();
        QString value = m_preferences->value(QString("DiceAlias_%1_value").arg(i),"").toString();
        bool replace = m_preferences->value(QString("DiceAlias_%1_type").arg(i),true).toBool();
        list->append(new DiceAlias(cmd,value,replace));
    }
    if(NULL!=m_receivedAlias)
    {
        /*foreach(int id,m_receivedAlias->keys())
        {*/
        // DiceAlias* dicealias = m_receivedAlias->value(id);
        list->append(*m_receivedAlias);
        //}
    }
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
    toolBar->addWidget(m_selectPersonComboBox);
    QAction* action = toolBar->addAction(QIcon::fromTheme("document-save", QIcon(":/resources/icons/save.png")),tr("save"), this, SLOT(save()));
    action->setToolTip(tr("Save all messages from this window in %1/%2.html").arg(m_preferences->value("ChatDirectory",QDir::homePath()).toString(), m_chat->name()));

    // SelectPersonComboBox
    scheduleUpdateChatMembers();

    QVBoxLayout * internalVLayout = new QVBoxLayout();
    internalVLayout->setMargin(0);
    internalVLayout->setSpacing(0);

    internalVLayout->addWidget(toolBar);
    internalVLayout->addWidget(m_editionZone);
    m_bottomWidget->setLayout(internalVLayout);

    m_splitter->addWidget(m_displayZone);
    m_splitter->addWidget(m_bottomWidget);

    QList<int> tailles;
    tailles.append(200);
    tailles.append(40);
    m_splitter->setSizes(tailles);

    setObjectName("ChatWindow");
    setAttribute(Qt::WA_DeleteOnClose, false);
    setLayout(vboxLayout);

    connect(m_displayZone,SIGNAL(detachView(bool)),this,SLOT(detachView(bool)));

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
void ChatWindow::manageDiceRoll(QString str,QString& messageTitle,QString& message)
{
    updateListAlias();

    QString localPersonIdentifier = m_selectPersonComboBox->itemData(m_selectPersonComboBox->currentIndex(), PlayersList::IdentifierRole).toString();
    Person * localPerson = PlayersList::instance()->getPerson(localPersonIdentifier);
    QColor color;
    if(m_diceParser->parseLine(str))
    {
        m_diceParser->Start();
        if(m_diceParser->getErrorMap().isEmpty())
        {
            messageTitle = tr("You");
            QString value;
            QString cmdLine;
            QString list;
            bool onlyValue = getMessageResult(value, cmdLine,list);
            color = localPerson->color();

            if(!onlyValue)
            {
                QString diceOutput = tr("got <span class=\"dice\">%1</span> at your dice roll [%3 (%4)]").arg(value).arg(cmdLine).arg(list);
                showMessage(messageTitle, color, diceOutput,NetMsg::DiceMessageAction);
                QString diceOutput2 = tr("got <span class=\"dice\">%1</span> [%3 (%4)]").arg(value).arg(cmdLine).arg(list);
                message = diceOutput2;
            }
            else
            {
                messageTitle="";
                showMessage(messageTitle, color,value,NetMsg::DiceMessageAction);
                message = value;
            }

        }
        else
        {
            QString messageCorps = m_diceParser->humanReadableError();
            messageTitle = tr("Syntax");
            color = Qt::red;
            showMessage(messageTitle, color, messageCorps);
        }
    }
    else
    {
        QString messageCorps = m_diceParser->humanReadableError();
        messageTitle = tr("Syntax");
        color = Qt::red;
        showMessage(messageTitle, color, messageCorps);
    }
}

// not (const QString & message), because we change it !
void ChatWindow::emettreTexte(QString messagehtml,QString message)
{
    //NetMsg::ChatMessageAction, NetMsg::DiceMessageAction, NetMsg::EmoteMessageAction
    NetMsg::Action action = NetMsg::DiceMessageAction;
    Q_UNUSED(messagehtml);

    bool ok=true;
    m_editionZone->clear();


    QString localPersonIdentifier = m_selectPersonComboBox->itemData(m_selectPersonComboBox->currentIndex(), PlayersList::IdentifierRole).toString();
    Person * localPerson = PlayersList::instance()->getPerson(localPersonIdentifier);

    QString tmpmessage=message.simplified();
    QString messageCorps="";
    QString messageTitle="";
    QColor color;

    if(m_operatorMap->contains(tmpmessage.left(1)))
    {
        CHAT_OPERATOR chatOperator = m_operatorMap->value(tmpmessage.left(1));
        tmpmessage=tmpmessage.remove(0,1);
        switch(chatOperator)
        {
        case DICEROLL:
            manageDiceRoll(tmpmessage,messageTitle,message);
            break;
        case SECRET_DICEROLL:
            manageDiceRoll(tmpmessage,messageTitle,message);
            return;
            break;
        case COMMAND:
        {
            int pos = tmpmessage.indexOf(' ');
            QString cmd = tmpmessage.left(pos);
            if(m_keyWordList.contains(cmd))
            {
                tmpmessage=tmpmessage.remove(0,pos);
                if (!m_warnedEmoteUnavailable && !m_chat->everyPlayerHasFeature(QString("Emote")))
                {
                    messageTitle = tr("Warning");
                    messageCorps = tr("Some users won't be enable to see your emotes.");
                    color = Qt::red;
                    showMessage(messageTitle, color, messageCorps);
                    m_warnedEmoteUnavailable = true;
                }


                showMessage(localPerson->name(), localPerson->color(), tmpmessage,NetMsg::EmoteMessageAction);
                action = NetMsg::EmoteMessageAction;
                break;

            }
        }

        }
    }
    else
    {//sending info to others.
        messageTitle = localPerson->name();
        showMessage(messageTitle, localPerson->color(), message);
        action = NetMsg::ChatMessageAction;
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
QString ChatWindow::diceToText(ExportedDiceResult& dice)
{
    QStringList resultGlobal;
    foreach(int face, dice.keys())
    {
        QStringList result;
        QStringList currentStreak;
        QList<QStringList> allStreakList;
        ListDiceResult diceResult =  dice.value(face);
        bool previousHighlight=false;
        QString patternColor("<span class=\"dice\">");
        foreach (DiceAndHighlight tmp, diceResult)
        {
            QStringList diceListStr;
            if((previousHighlight)&&(!tmp.second))
            {
                QStringList list;
                list << patternColor+currentStreak.join(',')+"</span>";
                allStreakList.append(list);
                currentStreak.clear();
            }
            else if((!previousHighlight)&&(tmp.second))
            {
                if(!currentStreak.isEmpty())
                {
                    QStringList list;
                    list << currentStreak.join(',');
                    allStreakList.append(list);
                    currentStreak.clear();
                }
            }
            previousHighlight = tmp.second;
            for(int i =0; i < tmp.first.size(); ++i)
            {
                quint64 dievalue = tmp.first[i];
                diceListStr << QString::number(dievalue);
            }
            if(diceListStr.size()>1)
            {
                QString first = diceListStr.takeFirst();
                first = QString("%1 [%2]").arg(first).arg(diceListStr.join(','));
                diceListStr.clear();
                diceListStr << first;
            }
            currentStreak << diceListStr.join(' ');
        }

        if(previousHighlight)
        {
            QStringList list;
            list <<  patternColor+currentStreak.join(',')+"</span>";
            allStreakList.append(list);
        }
        else
        {
            if(!currentStreak.isEmpty())
            {
                QStringList list;
                list << currentStreak.join(',');
                allStreakList.append(list);
            }
        }
        foreach(QStringList a, allStreakList)
        {
            result << a;
        }
        if(dice.keys().size()>1)
        {
            resultGlobal << QString(" d%2:(%1)").arg(result.join(",")).arg(face);
        }
        else
        {
            resultGlobal << result.join(",");
        }
    }
    return resultGlobal.join("");
}

bool ChatWindow::getMessageResult(QString& value, QString& command, QString& list)
{
    QString scalarText;
    QString diceText;
    //QString pattern("");



    bool hasDiceList = false;
    if(m_diceParser->hasDiceResult())
    {
        ExportedDiceResult list;
        m_diceParser->getLastDiceResult(list);//fills the ExportedDiceResult
        diceText = diceToText(list);
        hasDiceList= true;
    }
    if(m_diceParser->hasIntegerResultNotInFirst())
    {
        scalarText = tr("%1").arg(m_diceParser->getLastIntegerResult());
    }
    else if(hasDiceList)
    {
        scalarText = tr("%1").arg(m_diceParser->getSumOfDiceResult());
    }
    value=scalarText;
    list = diceText.trimmed();
    command = m_diceParser->getDiceCommand().toHtmlEscaped();
    if(m_diceParser->hasStringResult())
    {
        value = m_diceParser->getStringResult().replace("\n","<br/>");
        return true;
    }

    return false;
}

QAction * ChatWindow::toggleViewAction() const
{
    return m_toggleViewAction;
}


void ChatWindow::showMessage(const QString& user, const QColor& color, const QString& message, NetMsg::Action msgtype)
{
    m_displayZone->moveCursor(QTextCursor::End);
    m_displayZone->setTextColor(color);
    QString msg = message;
    QString pattern("%1 : %2");
    if(msgtype==NetMsg::EmoteMessageAction)
    {
        pattern = "<i>%1 %2</i>";
    }
    else if(msgtype==NetMsg::DiceMessageAction)
    {
        pattern = "%1 %2";
    }


    QString userSpan("<span style=\"color:%2;\">%1</span>");
    userSpan = userSpan.arg(user).arg(color.name());

    setUpdatesEnabled(false);

    m_displayZone->append(pattern.arg(userSpan).arg(msg));
    setUpdatesEnabled(true);
    if (!m_editionZone->hasFocus() && !m_hasUnseenMessage)
    {
        m_hasUnseenMessage = true;
        emit ChatWindowHasChanged(this);
    }

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
void ChatWindow::updateDiceAliases(QList<DiceAlias*>* map)
{
    m_receivedAlias = map;
}
void ChatWindow::detachView(bool b)
{
    if(NULL==m_window)
    {
        return;
    }
    static QMdiArea* parent = m_window->mdiArea();
    if(b)
    {
            m_window->setParent(NULL);
            m_window->setVisible(true);
    }
    else
    {
            //m_window->setParent(parent);
            parent->addSubWindow(m_window);
            m_window->setVisible(true);
    }
}
