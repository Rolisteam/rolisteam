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

#include "tchat.h"
#include "tchateditor.h"
//#include "types.h"

#include "tchatlistmodel.h"
#include "pluginmanager.h"
#include "dicesysteminterface.h"
#include <QDebug>
#include "message.h"
#include "rclient.h"

Tchat::Tchat(QWidget *parent)
: SubMdiWindows(parent)
{
    setObjectName("Tchat");
    m_clientList = new QList<Person*>;
    m_diceManager=DicePlugInManager::instance();
    m_listModel=new TchatListModel;

    setupUi();

    resize(200,300);

    m_active=false;
    m_historicNumber = 0;
    m_type = SubMdiWindows::TCHAT;
    setWindowIcon(QIcon(":/resources/icons/tchat_icon.png"));
    setWindowTitle(tr("Tchat"));
    m_messageHistoric.clear();

    connect(m_tchatEditor, SIGNAL(onEntry()), this, SLOT(onEntry()));
    connect(m_tchatEditor, SIGNAL(onArrowUp()), this, SLOT(getUpHistoric()));
    connect(m_tchatEditor, SIGNAL(onArrowDown()), this, SLOT(getDownHistoric()));
}


Tchat::~Tchat()
{
    delete m_listModel;
    delete m_listView;
    delete m_splitter;
}

void Tchat::setupUi()
{

    m_meetingRoom = new QTextEdit();
    m_meetingRoom->setReadOnly(true);
    m_meetingRoom->setMinimumHeight(30);

    m_tchatEditor = new TchatEditor();
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

void Tchat::showMessage(QString utilisateur, QColor couleur, QString message)
{
        m_meetingRoom->moveCursor(QTextCursor::End);
        m_meetingRoom->setTextColor(couleur);
        m_meetingRoom->append(tr("%1: ").arg(utilisateur));
        m_meetingRoom->setTextColor(Qt::black);
        m_meetingRoom->insertPlainText(message);
        m_meetingRoom->verticalScrollBar()->setSliderPosition(m_meetingRoom->verticalScrollBar()->maximum());
}
void Tchat::showDiceRoll(QString utilisateur, QColor couleur, QString message)
{
        m_meetingRoom->moveCursor(QTextCursor::End);
        m_meetingRoom->setTextColor(couleur);
        m_meetingRoom->append(tr("%1: ").arg(utilisateur));
        m_meetingRoom->setTextColor(Qt::black);
        m_meetingRoom->insertPlainText(message);
        m_meetingRoom->verticalScrollBar()->setSliderPosition(m_meetingRoom->verticalScrollBar()->maximum());
}
void Tchat::closeEvent(QCloseEvent *event)
{
	hide();
        event->ignore();
}
void Tchat::addPerson(Person* p)
{
    m_clientList->append(p);
}

void Tchat::removePerson(Person* p)
{
    m_clientList->removeOne(p);

}
void Tchat::setClients(QList<Person*>* tmp)
{
    m_listModel->setClients(tmp);
    m_listView->setModel(m_listModel);
}

void Tchat::getUpHistoric()
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
void Tchat::getDownHistoric()
{
        if (m_historicNumber < m_messageHistoric.size() - 1)
	{
                m_historicNumber++;
                m_tchatEditor->clear();
                m_tchatEditor->append(m_messageHistoric[m_historicNumber]);
	}
}
void Tchat::saveFile(QString &file)
{

        QTextDocument *document = m_meetingRoom->document();
	QString html = document->toHtml(QByteArray("UTF-8"));
        QTextStream textfile(&file);
        textfile << html;
}
void Tchat::openFile(QString& file)
{
    Q_UNUSED(file);
}

void Tchat::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_tchatEditor->setFocus(Qt::OtherFocusReason);
}


bool Tchat::defineMenu(QMenu* /*menu*/)
{
    return false;
}
void Tchat::onEntry()
{
    QString text= m_tchatEditor->document()->toPlainText();
    m_tchatEditor->document()->clear();
    QString item= m_combobox->currentText();
    DiceSystemInterface* tmp = m_diceManager->getInterface(item);
    qDebug() << "regexp=" << tmp->catchRegExp();
    QRegExp exp(tmp->catchRegExp());
    if(exp.indexIn(text)!=-1)
    {
        text = tmp->rollDice(m_tchatEditor->document()->toPlainText());
    }
    m_meetingRoom->append(text);
    /// todo: send messages
    // sendMessage();
    Message mtmp;
    QDataStream msg(&mtmp,QIODevice::ReadWrite);
    /// @todo: add chat ID, sender and few other stuff
    msg << objectName() << text;
    m_client->addMessageToSendQueue(mtmp);
}
