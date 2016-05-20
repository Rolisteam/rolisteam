/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
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
#include <QQuickItem>

#include <QJsonDocument>
#include <QJsonObject>

#include "charactersheetwindow.h"
#include "charactersheet/charactersheet.h"
#include "preferences/preferencesmanager.h"
#include "data/character.h"

#include "data/player.h"

CharacterSheetWindow::CharacterSheetWindow(CleverURI* uri,QWidget* parent)
    : MediaContainer(parent),m_localIsGM(false)
{
    m_uri=uri;
    m_title = tr("Character Sheet Viewer");
    // m_data = new QJsonObject();
    if(NULL==m_uri)
    {
        setCleverUriType(CleverURI::CHARACTERSHEET);
    }
    else
    {
        m_title.append("- %1").arg(m_uri->getData(ResourcesNode::NAME).toString());
    }
    setObjectName("CharacterSheetViewer");
    connect(&m_model,SIGNAL(characterSheetHasBeenAdded(CharacterSheet*)),this,SLOT(addTabWithSheetView(CharacterSheet*)));
    
    setWindowIcon(QIcon(":/resources/icons/treeview.png"));
    m_addSection = new QAction(tr("Add Section"),this);
    m_addLine= new QAction(tr("Add line"),this);
    m_addCharacterSheet= new QAction(tr("Add CharacterSheet"),this);
    m_copyTab = new QAction(tr("Copy Tab"),this);
    connect(m_copyTab,SIGNAL(triggered(bool)),this,SLOT(copyTab()));
    m_stopSharingTabAct = new QAction(tr("Stop Sharing"),this);

    m_loadQml = new QAction(tr("Load CharacterSheet View File"),this);

    m_detachTab = new QAction(tr("Detach Tabs"),this);
    m_view.setModel(&m_model);
    
    resize(m_preferences->value("charactersheetwindows/width",400).toInt(),m_preferences->value("charactersheetwindows/height",600).toInt());
    m_view.setAlternatingRowColors(true);
    setWindowTitle(m_title);
    
    m_tabs = new QTabWidget(this);
    m_tabs->addTab(&m_view,tr("Data"));
    setWidget(m_tabs);

    m_view.setContextMenuPolicy(Qt::CustomContextMenu);
    connect(&m_view,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayCustomMenu(QPoint)));
    
    connect(m_addLine,SIGNAL(triggered()),this,SLOT(addLine()));
    connect(m_addSection,SIGNAL(triggered()),this,SLOT(addSection()));
    connect(m_addCharacterSheet,SIGNAL(triggered()),this,SLOT(addCharacterSheet()));
    connect(m_loadQml,SIGNAL(triggered(bool)),this,SLOT(openQML()));
    connect(m_detachTab,SIGNAL(triggered(bool)),this,SLOT(detachTab()));

    m_imgProvider = new RolisteamImageProvider();
}
CharacterSheetWindow::~CharacterSheetWindow()
{
    if(NULL!=m_imgProvider)
    {
        delete m_imgProvider;
        m_imgProvider = NULL;
    }
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
    menu.addAction(m_loadQml);
    QModelIndex index = m_view.indexAt(pos);
    if(index.column()>0)
    {
        m_currentCharacterSheet = m_characterSheetlist.value(m_characterSheetlist.keys().at(index.column()-1));
        QMenu* affect = menu.addMenu(tr("Share To "));
        addSharingMenu(affect);
    }
    menu.exec(QCursor::pos());
}
void CharacterSheetWindow::addSharingMenu(QMenu* share)
{
    for(Character* character : PlayersList::instance()->getCharacterList())
    {
        QAction* action = share->addAction(QPixmap::fromImage(character->getAvatar()),character->getName());
        action->setData(character->getUuid());
        connect(action,SIGNAL(triggered(bool)),this,SLOT(affectSheetToCharacter()));
    }

}

void CharacterSheetWindow::contextMenuForTabs(const QPoint& pos)
{
    QMenu menu(this);

    m_currentCharacterSheet = m_characterSheetlist.values().at(m_tabs->currentIndex()-1);
    if(NULL!=m_currentCharacterSheet)
    {

        menu.addAction(m_detachTab);
        QMenu* share = menu.addMenu(tr("Share To"));

        menu.addAction(m_copyTab);
        menu.addAction(m_stopSharingTabAct);
        menu.addSeparator();
        addSharingMenu(share);

        menu.exec(QCursor::pos());
    }
}
bool CharacterSheetWindow::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::Hide)
    {
        QQuickWidget* wid = dynamic_cast<QQuickWidget*>(object);
        if(NULL!=wid)
        {
            if(-1==m_tabs->indexOf(wid))
            {
                wid->removeEventFilter(this);
                m_tabs->addTab(wid,wid->windowTitle());
            }
        }
        return MediaContainer::eventFilter(object,event);
    }
    return MediaContainer::eventFilter(object,event);
}

void CharacterSheetWindow::detachTab()
{
    QWidget* wid = m_tabs->currentWidget();
    QString title = m_tabs->tabBar()->tabText(m_tabs->currentIndex());
    m_tabs->removeTab(m_tabs->currentIndex());
    wid->installEventFilter(this);
    emit addWidgetToMdiArea(wid,title);
}
void CharacterSheetWindow::copyTab()
{
    QQuickWidget* wid = dynamic_cast<QQuickWidget*>(m_tabs->currentWidget());
    if(NULL!=wid)
    {
        CharacterSheet* sheet =m_characterSheetlist.value(wid);
        if(NULL!=sheet)
        {
            addTabWithSheetView(sheet);
        }
    }
}

void CharacterSheetWindow::affectSheetToCharacter()
{
    QAction* action = qobject_cast<QAction*>(sender());
    QString key = action->data().toString();
    Character* character = PlayersList::instance()->getCharacter(key);
    if(NULL!=character)
    {
        CharacterSheet* sheet = m_currentCharacterSheet;//m_model.getCharacterSheet(m_currentCharacterSheet);
        if(NULL!=sheet)
        {
            checkAlreadyShare(sheet);
            character->setSheet(sheet);
            sheet->setName(character->getName());
            m_tabs->setTabText(m_characterSheetlist.values().indexOf(sheet)+1,sheet->getName());

            Player* parent = character->getParentPlayer();
            Player* localItem =  PlayersList::instance()->getLocalPlayer();
            if((NULL!=parent)&&(NULL!=localItem)&&(localItem->isGM()))
            {
                m_sheetToPerson.insert(sheet,parent);
                NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::addCharacterSheet);
                msg.string8(character->getUuid());
                fill(&msg,sheet);
                Player* person = character->getParentPlayer();
                msg.sendTo(person->link());
            }
        }
    }
}
void CharacterSheetWindow::checkAlreadyShare(CharacterSheet* sheet)
{
    if(m_sheetToPerson.contains(sheet))
    {
        Player* olderParent = m_sheetToPerson.value(sheet);
        if(NULL!=olderParent)
        {
            NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::closeCharacterSheet);
            msg.string8(m_mediaId);
            msg.string8(sheet->getUuid());
            msg.sendTo(olderParent->link());
        }
        m_sheetToPerson.remove(sheet);
    }
}

void CharacterSheetWindow::addSection()
{
    m_model.addSection();
}
void CharacterSheetWindow::addCharacterSheet()
{
    m_model.addCharacterSheet();
}
void CharacterSheetWindow::addTabWithSheetView(CharacterSheet* chSheet)
{
    if(m_qmlData.isEmpty())
    {
        openQML();
    }
    m_qmlView = new QQuickWidget();
    m_qmlView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_qmlView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuForTabs(QPoint)));

    connect(m_qmlView->engine(),SIGNAL(warnings(QList<QQmlError>)),this,SLOT(displayError(QList<QQmlError>)));

    m_qmlView->engine()->addImageProvider(QLatin1String("rcs"),new RolisteamImageProvider());

    for(int i =0;i<chSheet->getFieldCount();++i)
    {
        CharacterSheetItem* field = chSheet->getFieldAt(i);
        if(NULL!=field)
        {
            m_qmlView->engine()->rootContext()->setContextProperty(field->getId(),field);
        }
    }

    //QString name(QStringLiteral("charactersheet.qml"));
    QTemporaryFile fileTemp;

    if(fileTemp.open())//QIODevice::WriteOnly
    {
        fileTemp.write(m_qmlData.toLatin1());
        fileTemp.close();
    }

    m_qmlView->setSource(QUrl(fileTemp.fileName()));
    m_qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    QObject* root = m_qmlView->rootObject();
    connect(root,SIGNAL(rollDiceCmd(QString)),this,SLOT(rollDice(QString)));

    m_characterSheetlist.insert(m_qmlView,chSheet);
    m_tabs->addTab(m_qmlView,chSheet->getTitle());
}
void CharacterSheetWindow::rollDice(QString str)
{
    QObject* obj = sender();
    QString label;
    for(QQuickWidget* qmlView : m_characterSheetlist.keys())
    {
        QObject* root = qmlView->rootObject();
        if(root == obj)
        {
            label = m_tabs->tabText(m_tabs->indexOf(qmlView));
        }
    }
    emit rollDiceCmd(str,label);
}

void CharacterSheetWindow::updateFieldFrom(CharacterSheet* sheet, CharacterSheetItem *item)
{
    if(NULL!=sheet)
    {
        NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::updateFieldCharacterSheet);
        msg.string8(m_mediaId);
        msg.string8(sheet->getUuid());
        QJsonObject object;
        item->saveDataItem(object);
        QJsonDocument doc;
        doc.setObject(object);
        msg.byteArray32(doc.toBinaryData());
        Player* person = NULL;
        if(m_localIsGM)
        {
            person = m_sheetToPerson.value(sheet);
        }
        else
        {
            person = PlayersList::instance()->getGM();
        }

        msg.sendTo(person->link());
    }
}
void CharacterSheetWindow::processUpdateFieldMessage(NetworkMessageReader* msg)
{
    QString idSheet = msg->string8();
    CharacterSheet* currentSheet=m_model.getCharacterSheetById(idSheet);

    if(NULL==currentSheet)
        return;

    QByteArray array= msg->byteArray32();
    if(array.isEmpty())
        return;


    QJsonDocument doc = QJsonDocument::fromBinaryData(array);
    QJsonObject obj = doc.object();
    currentSheet->setFieldData(obj);

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
    if((NULL!=m_uri)&&(m_uri->getUri().isEmpty()))
    {
       QString uri = QFileDialog::getSaveFileName(this, tr("Save Character Sheets Data"), m_preferences->value(QString("CharacterSheetDirectory"),QDir::homePath()).toString(),
                                                 tr("Character Sheets Data files (*.rcs)"));

       m_uri->setUri(uri);
    }
    saveMedia();

}
void CharacterSheetWindow::saveFile(QDataStream& stream)
{


    QJsonDocument json;
    QJsonObject obj;

    //fieldModel
    obj["data"]=m_data;


    obj["qml"]=m_qmlData;


    //background
    QJsonArray images;
    if(NULL!=m_imgProvider)
    {
        for(QPixmap pix : RolisteamImageProvider::getData()->values())
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix.save(&buffer, "PNG");
            images.append(QString(buffer.data().toBase64()));
        }
    }
    obj["background"]=images;

    //CharacterModel
    m_model.writeModel(obj,false);

    json.setObject(obj);
    //file.write(json.toJson());
    stream << json.toJson();
}


bool CharacterSheetWindow::openFile(const QString& fileUri)
{
    if(!fileUri.isEmpty())
    {
        QFile file(fileUri);
        if(file.open(QIODevice::ReadOnly))
        {
            QJsonDocument json = QJsonDocument::fromJson(file.readAll());
            QJsonObject jsonObj = json.object();

            m_data = jsonObj["data"].toObject();

            m_qmlData = jsonObj["qml"].toString();

            QJsonArray images = jsonObj["background"].toArray();
            int i = 0;
            for(auto jsonpix : images)
            {
                QString str = jsonpix.toString();
                QByteArray array = QByteArray::fromBase64(str.toUtf8());
                QPixmap pix;
                pix.loadFromData(array);
                //m_pixList.append(pix);
                m_imgProvider->insertPix(QStringLiteral("background_%1.jpg").arg(i),pix);
                ++i;
            }
            //m_model->load(data,m_canvasList);
            m_model.readModel(jsonObj,true);
        }
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

bool CharacterSheetWindow::getLocalIsGM() const
{
    return m_localIsGM;
}

void CharacterSheetWindow::setLocalIsGM(bool localIsGM)
{
    m_localIsGM = localIsGM;
}

RolisteamImageProvider *CharacterSheetWindow::getImgProvider() const
{
    return m_imgProvider;
}

void CharacterSheetWindow::setImgProvider(RolisteamImageProvider *imgProvider)
{
    m_imgProvider = imgProvider;
}

QString CharacterSheetWindow::getQmlData() const
{
    return m_qmlData;
}

void CharacterSheetWindow::setQmlData(const QString &qmlData)
{
    m_qmlData = qmlData;
}

void CharacterSheetWindow::addCharacterSheet(CharacterSheet* sheet)
{
    if(NULL!=sheet)
    {
        connect(sheet,SIGNAL(updateField(CharacterSheet*,CharacterSheetItem*)),this,SLOT(updateFieldFrom(CharacterSheet*,CharacterSheetItem*)));
        m_model.addCharacterSheet(sheet);
    }
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
    if(NULL!=m_uri)
    {
        setTitle(QStringLiteral("%1 - %2").arg(m_uri->getData(ResourcesNode::NAME).toString()).arg(tr("Character Sheet Viewer")));
        return openFile(m_uri->getUri());

    }
    else
    {
        return false;
    }
}
void CharacterSheetWindow::putDataIntoCleverUri()
{
    QByteArray data;
    QDataStream out(&data,QIODevice::WriteOnly);

    saveFile(out);
    if(NULL!=m_uri)
    {
        m_uri->setData(data);
    }
}

void CharacterSheetWindow::saveMedia()
{
    if((NULL!=m_uri)&&(m_uri->getUri().isEmpty()))
    {
        QString uri = m_uri->getUri();
        if(!uri.isEmpty())
        {
            QFile file(uri);
            if(!file.open(QIODevice::WriteOnly))
                return;

            QDataStream out(&file);

            saveFile(out);
        }
    }
}
void CharacterSheetWindow::fill(NetworkMessageWriter* msg,CharacterSheet* sheet,QString idChar)
{
    msg->string8(m_mediaId);
    msg->string8(idChar);
    msg->string8(m_title);
    if(NULL!=sheet)
    {
        sheet->fill(*msg);
    }
    msg->string32(m_qmlData);
    if(NULL!=m_imgProvider)
    {
        m_imgProvider->fill(*msg);
    }
    m_model.fillRootSection(msg);

}


void CharacterSheetWindow::read(NetworkMessageReader* msg)
{
    if(NULL==msg)
        return;

    QString characterId= msg->string8();
    Character* character = PlayersList::instance()->getCharacter(characterId);

    CharacterSheet* sheet = new CharacterSheet();
    if(NULL!=character)
    {
      character->setSheet(sheet);
    }

    m_mediaId = msg->string8();
    QString idChar = msg->string8();
    m_title = msg->string8();

    if(NULL!=sheet)
    {
        sheet->read(*msg);
    }
    m_qmlData = msg->string32();
    if(NULL!=m_imgProvider)
    {
        m_imgProvider->read(*msg);
    }

    addCharacterSheet(sheet);
    m_model.readRootSection(msg);

    Character* character = PlayersList::instance()->getCharacter(idChar);
    if(NULL!=character)
    {
        character->setSheet(sheet);
    }
}
/*
 *
 *    //m_customNetworkAccessFactory->insertQmlFile(QStringLiteral("rolisteam://charactersheet.qml"),m_qmlData);
    //m_qmlView->engine()->setNetworkAccessManagerFactory(m_customNetworkAccessFactory);

   // m_qmlView->setSource(QUrl(QStringLiteral("rolisteam://charactersheet.qml")));
    m_qmlView->engine()->addImportPath("qrc:/resources/qml");
    m_qmlView->engine()->addPluginPath("qrc:/resources/qml");
    m_qmlView->engine()->addImportPath("QtQuick");
    if(QFile::exists(name))
    {
        QFile::remove(name);
    }
    QFile file(name);
    QQmlComponent* com = new QQmlComponent(m_qmlView->engine(),this);
    const QUrl url;
    com->setData(m_qmlData.remove("import QtQuick 2.4\nimport \"qrc:/resources/qml/\"\n\n").toUtf8(),url);
    qDebug() << m_qmlData;
    m_qmlView->setContent(QUrl("Charactersheet.qml"),com,this);
 * void CharacterSheetWindow::continueLoading()
{
   // qDebug() << "ContinueLoading";
    if (m_sheetComponent->isError())
    {
        qWarning() << "hasError ///////"<< m_sheetComponent->errors();
    }
    else
    {
        QObject* sheetObj = m_sheetComponent->create();
        if(NULL!=sheetObj)
        {
           // qDebug() << "Creation succeed";
            QQuickItem* sheetItem = dynamic_cast<QQuickItem*>(sheetObj);
            if(NULL!=sheetItem)
            {
               // qDebug() << m_qmlView->rootObject();
                sheetItem->setParentItem(m_qmlView->rootObject());
            }
        }
        m_qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    }
}*/
