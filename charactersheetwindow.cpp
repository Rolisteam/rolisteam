/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
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
#include <QMenu>
#include <QFile>
#include <QFileDialog>
#include <QQmlContext>
#include <QQmlComponent>

#include <QJsonDocument>
#include <QJsonObject>

#include "charactersheetwindow.h"
#include "charactersheet/charactersheet.h"
#include "preferences/preferencesmanager.h"
#include "data/character.h"

#include "data/player.h"

CharacterSheetWindow::CharacterSheetWindow(CleverURI* uri,QWidget* parent)
    : MediaContainer(parent)
{
    m_uri=uri;
    m_title = tr("Character Sheet Viewer");
    if(NULL==m_uri)
    {
        setCleverUriType(CleverURI::CHARACTERSHEET);
    }
    setObjectName("CharacterSheetViewer");

    connect(&m_model,SIGNAL(characterSheetHasBeenAdded(CharacterSheet*)),this,SLOT(addTabWithSheetView(CharacterSheet*)));
    
    setWindowIcon(QIcon(":/resources/icons/treeview.png"));
    m_addSection = new QAction(tr("Add Section"),this);
    m_addLine= new QAction(tr("Add line"),this);
    m_addCharacterSheet= new QAction(tr("Add CharacterSheet"),this);
    m_saveCharacterSheet = new QAction(tr("Save Character Sheets"),this);
    m_openCharacterSheet= new QAction(tr("Open Character Sheets"),this);
    m_loadQml = new QAction(tr("Load CharacterSheet View File"),this);

    m_detachTab = new QAction(tr("Detach Tabs"),this);

    m_view.setModel(&m_model);
    
    resize(m_preferences->value("charactersheetwindows/width",400).toInt(),m_preferences->value("charactersheetwindows/height",200).toInt());
    m_view.setAlternatingRowColors(true);
    setWindowTitle(m_title);
    

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(&m_view,tr("Data"));
    setWidget(m_tabs);
    m_view.setContextMenuPolicy(Qt::CustomContextMenu);
    m_tabs->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tabs->tabBar(),SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuForTabs(QPoint)));
    
    
    connect(m_addLine,SIGNAL(triggered()),this,SLOT(addLine()));
    connect(m_addSection,SIGNAL(triggered()),this,SLOT(addSection()));
    connect(m_addCharacterSheet,SIGNAL(triggered()),this,SLOT(addCharacterSheet()));
    connect(m_saveCharacterSheet,SIGNAL(triggered()),this,SLOT(saveCharacterSheet()));
    connect(m_openCharacterSheet,SIGNAL(triggered()),this,SLOT(openCharacterSheet()));
    connect(&m_view,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayCustomMenu(QPoint)));
    connect(m_loadQml,SIGNAL(triggered(bool)),this,SLOT(openQML()));

    connect(m_detachTab,SIGNAL(triggered(bool)),this,SLOT(detachTab()));

    m_imgProvider = new RolisteamImageProvider();
}
CharacterSheetWindow::~CharacterSheetWindow()
{

}
void CharacterSheetWindow::addLine()
{
    m_model.addLine(m_view.currentIndex());
}
void CharacterSheetWindow::displayCustomMenu(const QPoint & pos)
{
    QMenu menu(this);
    
    menu.addAction(m_addLine);
    menu.addAction(m_addSection);
    menu.addAction(m_addCharacterSheet);
    menu.addSeparator();
    menu.addAction(m_openCharacterSheet);
    menu.addAction(m_saveCharacterSheet);
    menu.addSeparator();
    menu.addAction(m_loadQml);
    QModelIndex index = m_view.indexAt(pos);
    if(index.column()>0)
    {
        m_currentCharacterSheet = index.column()-1;
        QMenu* affect = menu.addMenu(tr("Affect To "));

        for(Character* character : PlayersList::instance()->getCharacterList())
        {
            QAction* action = affect->addAction(QPixmap::fromImage(character->getAvatar()),character->getName());
            action->setData(character->getUuid());
            connect(action,SIGNAL(triggered(bool)),this,SLOT(affectSheetToCharacter()));
        }

    }
    menu.exec(QCursor::pos());
}
void CharacterSheetWindow::contextMenuForTabs(const QPoint& pos)
{
    QMenu menu(this);

    m_currentCharacterSheet=m_tabs->tabBar()->tabAt(pos);
    menu.addAction(m_detachTab);
    if(m_currentCharacterSheet>0)
    {
        menu.exec(QCursor::pos());
    }


}
void CharacterSheetWindow::detachTab()
{
    QWidget* wid = m_tabs->widget(m_currentCharacterSheet);
    m_tabs->removeTab(m_currentCharacterSheet);
    emit addWidgetToMdiArea(wid);
}

void CharacterSheetWindow::affectSheetToCharacter()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString key = action->data().toString();
    Character* character = PlayersList::instance()->getCharacter(key);
    if(NULL!=character)
    {
        CharacterSheet* sheet = m_model.getCharacterSheet(m_currentCharacterSheet);
        if(NULL!=sheet)
        {
            character->setSheet(sheet);
            m_tabs->setTabText(m_currentCharacterSheet+1,character->getName());


            Player* parent = character->getParentPlayer();
            Player* localItem =  PlayersList::instance()->getLocalPlayer();
            if((NULL!=m_currentCharacterSheet)&&(NULL!=parent)&&(NULL!=localItem)&&(localItem->isGM()))
            {
                NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::addCharacterSheet);
                msg.string8(m_mediaId);
                sheet->fill(msg);
            }
        }
    }

}

void CharacterSheetWindow::addSection()
{
    m_model.addSection();
}
void CharacterSheetWindow::addCharacterSheet()
{
    addTabWithSheetView(m_model.addCharacterSheet());
}
void CharacterSheetWindow::addTabWithSheetView(CharacterSheet* chSheet)
{
    if(m_qmlData.isEmpty())
    {
        openQML();
    }
    QQuickWidget* qmlView = new QQuickWidget();
    //QFileInfo info(m_qmlUri);
    //qmlView->engine()->addPluginPath(info.absolutePath());
    qmlView->rootContext()->setContextProperty("_model",chSheet);
    connect(qmlView->engine(),SIGNAL(warnings(QList<QQmlError>)),this,SLOT(displayError(QList<QQmlError>)));

    //qmlView->setSource(QUrl::fromLocalFile(m_qmlUri));
    qmlView->engine()->addImageProvider("rcs",m_imgProvider);
    QQmlComponent compo(qmlView->engine());
    compo.setData(m_qmlData.toLatin1(),QUrl(""));
    compo.create();
    qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);

    m_characterSheetlist.append(qmlView);
    m_tabs->addTab(qmlView,chSheet->getTitle());
}

void CharacterSheetWindow::displayError(const QList<QQmlError> & warnings)
{
    foreach(auto error, warnings)
    {
        qDebug() << error.toString();
    }
}

void  CharacterSheetWindow::saveCharacterSheet()
{
    if(m_fileUri.isEmpty())
    {
        m_fileUri = QFileDialog::getSaveFileName(this, tr("Save Character Sheets Data"), m_preferences->value(QString("DataDirectory"),QVariant(".")).toString(),
                                                 tr("Character Sheets Data files (*.rcs)"));
    }
    saveFile(m_fileUri);
    
}
void CharacterSheetWindow::saveFile(const QString & file)
{
    if(!file.isEmpty())
    {
        QFile url(file);
        if(!url.open(QIODevice::ReadWrite | QIODevice::Text))
            return;

        QJsonDocument doc(QJsonDocument::fromJson(url.readAll()));
        QJsonObject jsonObj=doc.object();

        QJsonObject data = jsonObj["data"].toObject();

        m_model.writeModel(data,true);

        jsonObj["data"]=data;
        jsonObj["qml"]=m_qmlData;
        doc.setObject(jsonObj);

        url.write(doc.toJson());

        url.close();
    }
}

bool CharacterSheetWindow::openFile(const QString& fileUri)
{
    if(!fileUri.isEmpty())
    {
        QFile url(fileUri);
        if(!url.open(QIODevice::ReadOnly))
            return false;

        QJsonDocument doc(QJsonDocument::fromJson(url.readAll()));
        QJsonObject jsonObj=doc.object();

        QJsonObject data = jsonObj["data"].toObject();

        m_model.readModel(data);

        m_qmlData = jsonObj["qml"].toString();
        qDebug() << m_qmlData;

        QString str = jsonObj["background"].toString();
        QByteArray array = QByteArray::fromBase64(str.toUtf8());
        QPixmap pix;
        pix.loadFromData(array);
        m_imgProvider->insertPix("background.jpg",pix);

        for(auto widget : m_characterSheetlist)
        {
            widget->engine()->addImageProvider("rcs",m_imgProvider);
            QQmlComponent compo(widget->engine());

            compo.setData(m_qmlData.toLatin1(),QUrl(""));
            widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
        }

        url.close();
        return true;
    }
    return false;
}

void CharacterSheetWindow::openCharacterSheet()
{
    m_fileUri = QFileDialog::getOpenFileName(this, tr("Open Character Sheet data"), m_preferences->value(QString("DataDirectory"),QVariant(".")).toString(),
                                             tr("Character Sheet Data files (*.rcs)"));
    
    openFile(m_fileUri);
    
}
void CharacterSheetWindow::openQML()
{
    m_qmlUri = QFileDialog::getOpenFileName(this, tr("Open Character Sheets View"), m_preferences->value(QString("DataDirectory"),QVariant(".")).toString(),
                                             tr("Character Sheet files (*.qml)"));

    //m_qmlData;
    /// @todo load m_qmlUri into m_qmlData
    //openFile(m_fileUri);
}

void CharacterSheetWindow::closeEvent(QCloseEvent *event)
{
    setVisible(false);
    event->ignore();
    
}
bool CharacterSheetWindow::hasDockWidget() const
{
    return false;
}
QDockWidget* CharacterSheetWindow::getDockWidget()
{
    return NULL;
}
bool CharacterSheetWindow::readFileFromUri()
{
    return openFile(m_uri->getUri());
}

void CharacterSheetWindow::saveMedia()
{
    saveFile(m_uri->getUri());
}
