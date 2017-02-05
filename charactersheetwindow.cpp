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

    m_readOnlyAct = new QAction(tr("Read Only"),this);
    connect(m_readOnlyAct,SIGNAL(triggered(bool)),this,SLOT(setReadOnlyOnSelection()));


    m_loadQml = new QAction(tr("Load CharacterSheet View File"),this);

    m_detachTab = new QAction(tr("Detach Tabs"),this);
    m_view.setModel(&m_model);
    
    resize(m_preferences->value("charactersheetwindows/width",400).toInt(),m_preferences->value("charactersheetwindows/height",600).toInt());
    m_view.setAlternatingRowColors(true);
    m_view.setSelectionBehavior(QAbstractItemView::SelectItems);
    m_view.setSelectionMode(QAbstractItemView::ExtendedSelection);
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
void CharacterSheetWindow::setReadOnlyOnSelection()
{
    QList<QModelIndex> list = m_view.selectionModel()->selectedIndexes();
    bool allTheSame = true;
    int i=0;
    bool firstStatus;
    QList<CharacterSheetItem*> listItem;
    for(auto item : list)
    {
        if(0==item.column())
        {
            CharacterSheetItem* csitem = static_cast<CharacterSheetItem*>(item.internalPointer());
            if(NULL!=csitem)
            {
                listItem.append(csitem);
            }
        }
        else
        {
            CharacterSheet* sheet = m_model.getCharacterSheet(item.column()-1);
            if(NULL!=sheet)
            {
                CharacterSheetItem* csitem = static_cast<CharacterSheetItem*>(item.internalPointer());
                if(NULL!=csitem)
                {
                    listItem.append(sheet->getFieldFromKey(csitem->getId()));
                }
            }
        }
    }
    for(CharacterSheetItem* csitem : listItem)
    {
        if(NULL!=csitem)
        {
            if(i==0)
            {
                firstStatus = csitem->isReadOnly();
            }
            if(firstStatus != csitem->isReadOnly())
            {
                allTheSame = false;
            }
        }
        ++i;
    }
    bool valueToSet;
    if(allTheSame)
    {
        valueToSet = !firstStatus;
    }
    else
    {
        valueToSet = true;
    }
    for(CharacterSheetItem* csitem : listItem)
    {
         if(NULL!=csitem)
         {
            csitem->setReadOnly(valueToSet);
         }
    }
}

void CharacterSheetWindow::displayCustomMenu(const QPoint & pos)
{
    QMenu menu(this);

    menu.addAction(m_readOnlyAct);
    menu.addSeparator();
    menu.addAction(m_addLine);
    menu.addAction(m_addSection);
    menu.addAction(m_addCharacterSheet);
    menu.addSeparator();
    menu.addAction(m_loadQml);
    QModelIndex index = m_view.indexAt(pos);
    if(index.column()>0)
    {
        m_currentCharacterSheet = m_model.getCharacterSheet(index.column()-1);
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

    QWidget* wid = m_tabs->currentWidget();
    QQuickWidget* quickWid=dynamic_cast<QQuickWidget*>(wid);
     m_currentCharacterSheet =  m_characterSheetlist.value(quickWid);

    //m_currentCharacterSheet = m_characterSheetlist.values().at(m_tabs->currentIndex()-1);
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
            QQuickWidget* wid = m_characterSheetlist.key(sheet);
            m_tabs->setTabText(m_tabs->indexOf(wid),sheet->getName());

            Player* parent = character->getParentPlayer();
            Player* localItem =  PlayersList::instance()->getLocalPlayer();
            if((NULL!=parent)&&(NULL!=localItem)&&(localItem->isGM()))
            {
                m_sheetToPerson.insert(sheet,parent);
                NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::addCharacterSheet);
                //msg.string8(character->getUuid());
                fill(&msg,sheet,character->getUuid());
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
            PlayersList* list = PlayersList::instance();
            if(list->hasPlayer(olderParent))
            {
                msg.sendTo(olderParent->link());
            }
        }
        m_sheetToPerson.remove(sheet);
    }
}
void CharacterSheetWindow::removeConnection(Player* player)
{
    CharacterSheet* key = m_sheetToPerson.key(player,NULL);
    if(NULL!=key)
    {
        m_sheetToPerson.remove(key);
    }
}

void CharacterSheetWindow::addSection()
{
    m_model.addSection();
}
void CharacterSheetWindow::addCharacterSheet()
{
    CharacterSheet* sheet = m_model.addCharacterSheet();

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


    m_qmlView->setSource(QUrl::fromLocalFile(fileTemp.fileName()));
    m_qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    readErrorFromQML(m_qmlView->errors());

    QObject* root = m_qmlView->rootObject();
    connect(root,SIGNAL(rollDiceCmd(QString)),this,SLOT(rollDice(QString)));
    m_characterSheetlist.insert(m_qmlView,chSheet);
    m_tabs->addTab(m_qmlView,chSheet->getTitle());
}
void CharacterSheetWindow::readErrorFromQML(QList<QQmlError> list)
{
    for(auto error : list)
    {
        emit errorOccurs(error.toString(),MainWindow::Error);
    }
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
        if(NULL!=person)
        {
            msg.sendTo(person->link());
        }
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
    QString result="";
    for(auto error : warnings)
    {
        result += error.toString();
    }
    QMessageBox::information(this,tr("Errors QML"),result,QMessageBox::Ok);
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
QJsonDocument CharacterSheetWindow::saveFile()
{


    QJsonDocument json;
    QJsonObject obj;

    //Get datamodel
    obj["data"]=m_data;

    //qml file
    obj["qml"]=m_qmlData;


    //background
    QJsonArray images;
    QHash<QString,QPixmap>* hash = RolisteamImageProvider::getData();
    for(auto key : m_pixmapList.keys())
    {
        QJsonObject obj;
        obj["key"]=key;
        QPixmap* pix = m_pixmapList.value(key);
        if(NULL!=pix)
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix->save(&buffer, "PNG");
            obj["bin"]=QString(buffer.data().toBase64());
            images.append(obj);
        }
    }
    obj["background"]=images;
    m_model.writeModel(obj,false);
    json.setObject(obj);
    return json;
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
                QJsonObject obj = jsonpix.toObject();
                QString str = obj["bin"].toString();
                QString key = obj["key"].toString();
                QByteArray array = QByteArray::fromBase64(str.toUtf8());
                QPixmap* pix = new QPixmap();
                pix->loadFromData(array);
                m_imgProvider->insertPix(QStringLiteral("%1").arg(key),*pix);
                m_pixmapList.insert(key,pix);
                ++i;
            }
            //m_model->load(data,m_canvasList);
            m_model.readModel(jsonObj,true);
            for(int j = 0; j< m_model.getCharacterSheetCount(); ++j)
            {
                CharacterSheet* sheet = m_model.getCharacterSheet(j);
                if(NULL!=sheet)
                {
                    connect(sheet,SIGNAL(updateField(CharacterSheet*,CharacterSheetItem*)),this,SLOT(updateFieldFrom(CharacterSheet*,CharacterSheetItem*)));
                }
            }
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

    if(!m_qmlUri.isEmpty())
    {
        QFile file(m_qmlUri);
        if(file.open(QIODevice::ReadOnly))
        {
            m_qmlData = QString(file.readAll());

        }
    }
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
    QJsonDocument doc = saveFile();
    data = doc.toBinaryData();
    if(NULL!=m_uri)
    {
        m_uri->setData(data);
    }
}

void CharacterSheetWindow::saveMedia()
{
    if((NULL!=m_uri)&&(!m_uri->getUri().isEmpty()))
    {
        QString uri = m_uri->getUri();
        if(!uri.isEmpty())
        {
            if(!uri.endsWith(".rcs"))
            {
                uri += QLatin1String(".rcs");
            }
            QFile file(uri);
            if(file.open(QIODevice::WriteOnly))
            {
                QJsonDocument doc = saveFile();
                file.write(doc.toJson());
                file.close();
            }
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

    CharacterSheet* sheet = new CharacterSheet();

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
