/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni                              *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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



#include <QComboBox>
#include <QIcon>
#include <QListView>
#include <QScrollBar>
#include <QTextStream>
#include <QVBoxLayout>

#include "chat.h"
#include "chateditor.h"
//#include "types.h"

#include "chatlistmodel.h"
#include "pluginmanager.h"
#include "dicesysteminterface.h"
#include <QDebug>
#include "message.h"
#include "rclient.h"


Chat::Chat(CleverURI* uri,QWidget *parent)
: SubMdiWindows(parent)
{
    setObjectName("Tchat");
    m_clientList = new QList<Person*>;
    m_diceManager=DicePlugInManager::instance();
    m_listModel=new ChatListModel;

    setupUi();

    resize(200,300);

    m_active=false;
    m_historicNumber = 0;
    m_uri = uri;
    setWindowIcon(QIcon(":/resources/icons/tchat_icon.png"));
    setWindowTitle(tr("Tchat"));
    m_messageHistoric.clear();

    connect(m_tchatEditor, SIGNAL(onEntry()), this, SLOT(onEntry()));
    connect(m_tchatEditor, SIGNAL(onArrowUp()), this, SLOT(getUpHistoric()));
    connect(m_tchatEditor, SIGNAL(onArrowDown()), this, SLOT(getDownHistoric()));
}


Chat::~Chat()
{
    delete m_listModel;
    delete m_listView;
    delete m_splitter;
}

void Chat::setupUi()
{

    m_meetingRoom = new QTextEdit();
    m_meetingRoom->setReadOnly(true);
    m_meetingRoom->setMinimumHeight(30);

    m_tchatEditor = new ChatEditor();
    m_tchatEditor->setReadOnly(false);
    m_tchatEditor->setMinimumHeight(30);
    m_tchatEditor->setAcceptRichText(false);

    m_combobox=new QComboBox;
    m_combobox->addItems(m_diceManager->getInterfaceList());

    m_listView=new QListView(this);

    QVBoxLayout* layout=new QVBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_combobox);
    layout->addWidget(m_listView);

    m_splitter = new QSplitter;

    m_HorizonSplitter = new QSplitter;
    m_splitter->setOrientation(Qt::Vertical);
    m_HorizonSplitter->setOrientation(Qt::Horizontal);


    m_HorizonSplitter->addWidget(m_meetingRoom);

    QWidget* t = new QWidget();
    t->setLayout(layout);

    m_HorizonSplitter->addWidget(t);

    m_splitter->setChildrenCollapsible(false);

    m_splitter->addWidget(m_HorizonSplitter);
    m_splitter->addWidget(m_tchatEditor);

    m_HorizonSplitter->setStretchFactor(0,2);
    m_HorizonSplitter->setStretchFactor(1,1);

    m_splitter->setStretchFactor(0,5);
    m_splitter->setStretchFactor(1,1);
    setWidget(m_splitter);
}

void Chat::showMessage(QString utilisateur, QColor couleur, QString message)
{
        m_meetingRoom->moveCursor(QTextCursor::End);
        m_meetingRoom->setTextColor(couleur);
        m_meetingRoom->append(tr("%1: ").arg(utilisateur));
        m_meetingRoom->setTextColor(Qt::black);
        m_meetingRoom->insertPlainText(message);
        m_meetingRoom->verticalScrollBar()->setSliderPosition(m_meetingRoom->verticalScrollBar()->maximum());
}
void Chat::showDiceRoll(QString utilisateur, QColor couleur, QString message)
{
        m_meetingRoom->moveCursor(QTextCursor::End);
        m_meetingRoom->setTextColor(couleur);
        m_meetingRoom->append(tr("%1: ").arg(utilisateur));
        m_meetingRoom->setTextColor(Qt::black);
        m_meetingRoom->insertPlainText(message);
        m_meetingRoom->verticalScrollBar()->setSliderPosition(m_meetingRoom->verticalScrollBar()->maximum());
}
void Chat::closeEvent(QCloseEvent *event)
{
	hide();
        event->ignore();
}
void Chat::addPerson(Person* p)
{
    m_clientList->append(p);
}

void Chat::removePerson(Person* p)
{
    m_clientList->removeOne(p);

}
void Chat::setClients(QList<Person*>* tmp)
{
    m_listModel->setClients(tmp);
    m_listView->setModel(m_listModel);
}

void Chat::getUpHistoric()
{
        if (m_messageHistoric.isEmpty())
                return;

        if (m_historicNumber == m_messageHistoric.size())
                m_messageHistoric.append(m_tchatEditor->toPlainText());

        if (m_historicNumber)
                m_historicNumber--;

        m_tchatEditor->clear();
        m_tchatEditor->append(m_messageHistoric[m_historicNumber]);
}
void Chat::getDownHistoric()
{
        if (m_historicNumber < m_messageHistoric.size() - 1)
	{
                m_historicNumber++;
                m_tchatEditor->clear();
                m_tchatEditor->append(m_messageHistoric[m_historicNumber]);
	}
}
void Chat::saveFile(const QString &file)
{

        QTextDocument *document = m_meetingRoom->document();
	QString html = document->toHtml(QByteArray("UTF-8"));
        QString tmp= file;
        QTextStream textfile(&tmp);
        textfile << html;
}
void Chat::openFile(const QString& file)
{
    Q_UNUSED(file);
}

void Chat::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_tchatEditor->setFocus(Qt::OtherFocusReason);
}


bool Chat::defineMenu(QMenu* /*menu*/)
{
    return false;
}
void Chat::onEntry()
{
    QString text= m_tchatEditor->document()->toPlainText();
    m_tchatEditor->document()->clear();
    QString item= m_combobox->currentText();
    DiceSystemInterface* tmp = m_diceManager->getInterface(item);

    QRegExp exp(tmp->catchRegExp());
    if(exp.indexIn(text)!=-1)
    {
        text = tmp->rollDice(m_tchatEditor->document()->toPlainText());
    }
    m_meetingRoom->append(text);
    /// todo: send messages
    // sendMessage();
    Message* mtmp = new Message;
    QByteArray* tmpArray = mtmp->getDataArray();

    QDataStream msg(tmpArray,QIODevice::WriteOnly);
    /// @todo: add chat ID, sender and few other stuff

  //  mtmp->setCategory(1);
    msg << objectName() << text;

    m_client->addMessageToSendQueue(mtmp);
}
bool Chat::hasDockWidget() const
{
    return false;
}
QDockWidget* Chat::getDockWidget()
{
    return NULL;
}
