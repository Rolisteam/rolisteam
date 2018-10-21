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
#include <QPrintDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlProperty>
#include <QPrinter>
#include <QToolButton>
#include <QAction>
#include <QContextMenuEvent>

#include "charactersheetwindow.h"
#include "charactersheet/charactersheet.h"
#include "preferences/preferencesmanager.h"
#include "data/character.h"

#include "data/player.h"

CharacterSheetWindow::CharacterSheetWindow(CleverURI* uri,QWidget* parent)
    : MediaContainer(parent)
{
    m_uri=uri;
    if(nullptr==m_uri)
    {
        setCleverUriType(CleverURI::CHARACTERSHEET);
    }

    setObjectName("CharacterSheetViewer");
    connect(&m_model,SIGNAL(characterSheetHasBeenAdded(CharacterSheet*)),this,SLOT(addTabWithSheetView(CharacterSheet*)));
    
    setWindowIcon(QIcon(":/resources/icons/treeview.png"));
    m_addSection = new QAction(tr("Add Section"),this);
    m_addLine= new QAction(tr("Add Field"),this);
    m_addCharacterSheet= new QAction(tr("Add CharacterSheet"),this);
    m_copyTab = new QAction(tr("Copy Tab"),this);
    connect(m_copyTab,SIGNAL(triggered(bool)),this,SLOT(copyTab()));
    m_stopSharingTabAct = new QAction(tr("Stop Sharing"),this);

    m_readOnlyAct = new QAction(tr("Read Only"),this);
    connect(m_readOnlyAct,SIGNAL(triggered(bool)),this,SLOT(setReadOnlyOnSelection()));


    m_loadQml = new QAction(tr("Load CharacterSheet View File"),this);

    m_detachTab = new QAction(tr("Detach Tabs"),this);
    m_printAct = new QAction(tr("Print Page"),this);
    m_view.setModel(&m_model);
    
    resize(m_preferences->value("charactersheetwindows/width",400).toInt(),m_preferences->value("charactersheetwindows/height",600).toInt());
    m_view.setAlternatingRowColors(true);
    m_view.setSelectionBehavior(QAbstractItemView::SelectItems);
    m_view.setSelectionMode(QAbstractItemView::ExtendedSelection);
    updateTitle();
    m_tabs = new QTabWidget(this);
    m_tabs->addTab(&m_view,tr("Data"));
    setWidget(m_tabs);

    m_view.setContextMenuPolicy(Qt::CustomContextMenu);
    connect(&m_view,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayCustomMenu(QPoint)));
    
    connect(m_addLine,SIGNAL(triggered()),this,SLOT(addLine()));
    connect(m_addSection,SIGNAL(triggered()),this,SLOT(addSection()));
    connect(m_addCharacterSheet,SIGNAL(triggered()),this,SLOT(addCharacterSheet()));
    connect(m_loadQml,SIGNAL(triggered(bool)),this,SLOT(openQML()));
    connect(m_printAct,&QAction::triggered,this,&CharacterSheetWindow::exportPDF);
    connect(m_detachTab,SIGNAL(triggered(bool)),this,SLOT(detachTab()));

    m_imgProvider = new RolisteamImageProvider();
    m_pixmapList = QSharedPointer<QHash<QString,QPixmap>>(new QHash<QString,QPixmap>());
    m_imgProvider->setData(m_pixmapList);


    auto button = new QToolButton();//tr("Actions")
    auto act = new QAction(QIcon("://resources/icons/list.svg"), tr("Actions"),this);
    button->setDefaultAction(act);
    m_tabs->setCornerWidget(button);
    connect(button,&QPushButton::clicked,this,[button,this](){
        contextMenuForTabs(QPoint(button->pos().x(),0));
    });
}
CharacterSheetWindow::~CharacterSheetWindow()
{
    if(nullptr!=m_imgProvider)
    {
        delete m_imgProvider;
        m_imgProvider = nullptr;
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
    bool firstStatus = true;
    QList<CharacterSheetItem*> listItem;
    for(auto item : list)
    {
        if(0==item.column())
        {
            CharacterSheetItem* csitem = static_cast<CharacterSheetItem*>(item.internalPointer());
            if(nullptr!=csitem)
            {
                listItem.append(csitem);
            }
        }
        else
        {
            CharacterSheet* sheet = m_model.getCharacterSheet(item.column()-1);
            if(nullptr!=sheet)
            {
                CharacterSheetItem* csitem = static_cast<CharacterSheetItem*>(item.internalPointer());
                if(nullptr!=csitem)
                {
                    listItem.append(sheet->getFieldFromKey(csitem->getId()));
                }
            }
        }
    }
    for(CharacterSheetItem* csitem : listItem)
    {
        if(nullptr!=csitem)
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
        if(nullptr!=csitem)
        {
            csitem->setReadOnly(valueToSet);
        }
    }
}

void CharacterSheetWindow::updateTitle()
{
    setWindowTitle(tr("%1 - (Character Sheet Viewer)").arg(getUriName()));
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

    addActionToMenu(menu);
    menu.exec(QCursor::pos());
}
void CharacterSheetWindow::addSharingMenu(QMenu* share)
{
    for(Character* character : PlayersList::instance()->getCharacterList())
    {
        QAction* action = share->addAction(QPixmap::fromImage(character->getAvatar()),character->name());
        action->setData(character->getUuid());
        connect(action,SIGNAL(triggered(bool)),this,SLOT(affectSheetToCharacter()));
    }

}

void CharacterSheetWindow::contextMenuForTabs(const QPoint &pos)
{
    QMenu menu(this);

    QWidget* wid = m_tabs->currentWidget();
    QQuickWidget* quickWid=dynamic_cast<QQuickWidget*>(wid);
    m_currentCharacterSheet =  m_characterSheetlist.value(quickWid);

    if(nullptr!=m_currentCharacterSheet)
    {

        menu.addAction(m_detachTab);
        QMenu* share = menu.addMenu(tr("Share To"));

        menu.addAction(m_copyTab);
        menu.addAction(m_stopSharingTabAct);
        menu.addSeparator();
        addSharingMenu(share);
        addActionToMenu(menu);
        menu.addAction(m_printAct);

        menu.exec(quickWid->mapToGlobal(pos));
    }
}
bool CharacterSheetWindow::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::Hide)
    {
        QQuickWidget* wid = dynamic_cast<QQuickWidget*>(object);
        if(nullptr!=wid)
        {
            if(-1==m_tabs->indexOf(wid))
            {
                wid->removeEventFilter(this);
                m_tabs->addTab(wid,wid->windowTitle());
            }
        }
        return MediaContainer::eventFilter(object,event);
    }
    /*else if(event->type() == QEvent::MouseButtonPress)
    {
        qDebug() << "mouse Press";
    }
    else if (event->type() == QEvent::ContextMenu)
    {
       // QQuickItem* wid = dynamic_cast<QQuickItem*>(object);
        auto menuEvent = dynamic_cast<QContextMenuEvent*>(event);
        if( nullptr != menuEvent)//nullptr != wid &&
        {
            contextMenuForTabs(menuEvent->pos());
        }
        qDebug() << "Context Menu";
        return false;
    }*/
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
    if(nullptr!=wid)
    {
        CharacterSheet* sheet =m_characterSheetlist.value(wid);
        if(nullptr!=sheet)
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
    if(nullptr!=character)
    {
        CharacterSheet* sheet = m_currentCharacterSheet;//m_model.getCharacterSheet(m_currentCharacterSheet);
        if(nullptr!=sheet)
        {
            checkAlreadyShare(sheet);
            character->setSheet(sheet);
            sheet->setName(character->name());
            QQuickWidget* wid = m_characterSheetlist.key(sheet);
            m_tabs->setTabText(m_tabs->indexOf(wid),sheet->getName());

            Player* parent = character->getParentPlayer();
            Player* localItem =  PlayersList::instance()->getLocalPlayer();
            if((nullptr!=parent)&&(nullptr!=localItem)&&(localItem->isGM()))
            {
                m_sheetToPerson.insert(sheet,parent);

                Player* person = character->getParentPlayer();
                if(nullptr != person)
                {
                    NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::addCharacterSheet);
                    QStringList idList;
                    idList << person->getUuid();
                    msg.setRecipientList(idList,NetworkMessage::OneOrMany);
                    msg.string8(parent->getUuid());
                    fill(&msg,sheet,character->getUuid());
                    msg.sendToServer();
                }
            }
        }
    }
}
void CharacterSheetWindow::checkAlreadyShare(CharacterSheet* sheet)
{
    if(m_sheetToPerson.contains(sheet))
    {
        Player* olderParent = m_sheetToPerson.value(sheet);
        if(nullptr!=olderParent)
        {
            NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::closeCharacterSheet);
            QStringList recipiants;
            recipiants << olderParent->getUuid();
            msg.setRecipientList(recipiants,NetworkMessage::OneOrMany);
            msg.string8(m_mediaId);
            msg.string8(sheet->getUuid());
            PlayersList* list = PlayersList::instance();
            if(list->hasPlayer(olderParent))
            {
                msg.sendToServer();
            }
        }
        m_sheetToPerson.remove(sheet);
    }
}
bool CharacterSheetWindow::hasCharacterSheet(QString id)
{
    if(nullptr==m_model.getCharacterSheetById(id))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void CharacterSheetWindow::removeConnection(Player* player)
{
    CharacterSheet* key = m_sheetToPerson.key(player,nullptr);
    if(nullptr!=key)
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
    m_model.addCharacterSheet();
}
void CharacterSheetWindow::addTabWithSheetView(CharacterSheet* chSheet)
{
    if(m_qmlData.isEmpty())
    {
        openQML();
    }
    auto qmlView = new QQuickWidget(this);

    auto imageProvider = new RolisteamImageProvider();
    imageProvider->setData(m_imgProvider->getData());

    auto engineQml = qmlView->engine();

    engineQml->addImageProvider(QLatin1String("rcs"),imageProvider);

    if(m_sheetToCharacter.contains(chSheet))
    {
        auto character = m_sheetToCharacter.value(chSheet);
        character->setImageProvider(imageProvider);
        qmlView->engine()->rootContext()->setContextProperty("_character",character);
    }

    for(int i =0;i<chSheet->getFieldCount();++i)
    {
        CharacterSheetItem* field = chSheet->getFieldAt(i);
        if(nullptr!=field)
        {
            qmlView->engine()->rootContext()->setContextProperty(field->getId(),field);
        }
    }

    QTemporaryFile fileTemp;

    if(fileTemp.open())//QIODevice::WriteOnly
    {
        fileTemp.write(m_qmlData.toUtf8());
        fileTemp.close();
    }


    qmlView->setSource(QUrl::fromLocalFile(fileTemp.fileName()));
    qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    auto window = qmlView->rootObject();
    if(nullptr != window)
        //window->installEventFilter(this);
    //qmlView->setContextMenuPolicy(Qt::CustomContextMenu);
    //connect(qmlView, &QQuickWidget::customContextMenuRequested,this,&CharacterSheetWindow::contextMenuForTabs);
    readErrorFromQML(qmlView->errors());
    displayError(qmlView->errors());

    QObject* root = qmlView->rootObject();

    // CONNECTION TO SIGNAL FROM QML CHARACTERSHEET
    connect(root,SIGNAL(rollDiceCmd(QString,bool)),this,SLOT(rollDice(QString,bool)));
    connect(root,SIGNAL(rollDiceCmd(QString)),this,SLOT(rollDice(QString)));
    //connect(root,SIGNAL(showContextMenu(int,int)),this,SLOT(contextMenuForTabs(int,int)));

    m_characterSheetlist.insert(qmlView,chSheet);
    int id = m_tabs->addTab(qmlView,chSheet->getTitle());
    auto tabBar = m_tabs->tabBar();
    /* if(nullptr != tabBar)
    {

    }*/
    if(!m_localIsGM)
    {
        m_tabs->setCurrentIndex(id);
    }
}
void CharacterSheetWindow::readErrorFromQML(QList<QQmlError> list)
{
    for(auto error : list)
    {
        emit errorOccurs(error.toString());
    }
}

void CharacterSheetWindow::rollDice(QString str,bool alias)
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
    emit rollDiceCmd(str,label,alias);
}

void CharacterSheetWindow::updateFieldFrom(CharacterSheet* sheet, CharacterSheetItem *item)
{
    if(nullptr!=sheet)
    {
        Player* person = nullptr;
        if(m_localIsGM)
        {
            person = m_sheetToPerson.value(sheet);
        }
        else
        {
            person = PlayersList::instance()->getGM();
        }
        if(nullptr!=person)
        {
            NetworkMessageWriter msg(NetMsg::CharacterCategory,NetMsg::updateFieldCharacterSheet);
            QStringList recipiants;
            recipiants << person->getUuid();
            msg.setRecipientList(recipiants,NetworkMessage::OneOrMany);
            msg.string8(m_mediaId);
            msg.string8(sheet->getUuid());
            QJsonObject object;
            item->saveDataItem(object);
            QJsonDocument doc;
            doc.setObject(object);
            msg.byteArray32(doc.toBinaryData());
            msg.sendToServer();
        }
    }
}
void CharacterSheetWindow::processUpdateFieldMessage(NetworkMessageReader* msg)
{
    QString idSheet = msg->string8();
    CharacterSheet* currentSheet=m_model.getCharacterSheetById(idSheet);

    if(nullptr==currentSheet)
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
    if(!warnings.isEmpty())
    {
        QMessageBox::information(this,tr("QML Errors"),result,QMessageBox::Ok);
    }
}

void  CharacterSheetWindow::saveCharacterSheet()
{
    if((nullptr!=m_uri)&&(m_uri->getUri().isEmpty()))
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

    if(!m_fileUri.isEmpty())
    {
        QFile file(m_fileUri);
        if(file.open(QIODevice::ReadOnly))
        {
            json = QJsonDocument::fromJson(file.readAll());
        }
        obj = json.object();
    }

    //Get datamodel
    obj["data"]=m_data;

    //qml file
    obj["qml"]=m_qmlData;

    //background
    QJsonArray images;
    for(auto key : m_pixmapList->keys())
    {
        QJsonObject obj;
        obj["key"]=key;
        QPixmap pix = m_pixmapList->value(key);
        if(!pix.isNull())
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            pix.save(&buffer, "PNG");
            obj["bin"]=QString(buffer.data().toBase64());
            images.append(obj);
        }
    }
    obj["background"]=images;
    m_model.writeModel(obj,false);
    json.setObject(obj);
    return json;
}

bool CharacterSheetWindow::readData(QByteArray data)
{
    QJsonDocument json = QJsonDocument::fromJson(data);
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
        m_imgProvider->insertPix(key,*pix);
        ++i;
    }

    const auto fonts = jsonObj["fonts"].toArray();
    for(const auto obj : fonts)
    {
        const auto font = obj.toObject();
        const auto fontData = QByteArray::fromBase64(font["data"].toString("").toLatin1());
        QFontDatabase::addApplicationFontFromData(fontData);
    }

    m_model.readModel(jsonObj,true);
    for(int j = 0; j< m_model.getCharacterSheetCount(); ++j)
    {
        CharacterSheet* sheet = m_model.getCharacterSheet(j);
        if(nullptr!=sheet)
        {
            connect(sheet,SIGNAL(updateField(CharacterSheet*,CharacterSheetItem*)),this,SLOT(updateFieldFrom(CharacterSheet*,CharacterSheetItem*)));
        }
    }
    return true;
}

bool CharacterSheetWindow::openFile(const QString& fileUri)
{
    if(!fileUri.isEmpty())
    {
        QFile file(fileUri);
        if(file.open(QIODevice::ReadOnly))
        {
            readData(file.readAll());
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
    if(nullptr!=sheet)
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
    return nullptr;
}
bool CharacterSheetWindow::readFileFromUri()
{
    if(nullptr!=m_uri)
    {
        updateTitle();

        if(m_uri->getCurrentMode() == CleverURI::Internal || !m_uri->exists())
        {
            QByteArray data = m_uri->getData();
            m_uri->setCurrentMode(CleverURI::Internal);
            QJsonDocument doc = QJsonDocument::fromBinaryData(data);
            return readData(doc.toJson());
        }
        else if(m_uri->getCurrentMode() == CleverURI::Linked)
        {
            return openFile(m_uri->getUri());
        }
    }
    return false;
}
void CharacterSheetWindow::putDataIntoCleverUri()
{
    QByteArray data;
    QJsonDocument doc = saveFile();
    data = doc.toBinaryData();
    if(nullptr!=m_uri)
    {
        m_uri->setData(data);
    }
}

void CharacterSheetWindow::saveMedia()
{
    if((nullptr!=m_uri)&&(!m_uri->getUri().isEmpty()))
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
    msg->string8(getUriName());


    if(nullptr!=sheet)
    {
        sheet->fill(*msg);
    }
    msg->string32(m_qmlData);
    if(nullptr!=m_imgProvider)
    {
        m_imgProvider->fill(*msg);
    }
    m_model.fillRootSection(msg);

}

void CharacterSheetWindow::readMessage(NetworkMessageReader& msg)
{
    CharacterSheet* sheet = new CharacterSheet();

    m_mediaId = msg.string8();
    QString idChar = msg.string8();
    setUriName(msg.string8());

    if(nullptr!=sheet)
    {
        sheet->read(msg);
    }
    m_qmlData = msg.string32();
    if(nullptr!=m_imgProvider)
    {
        m_imgProvider->read(msg);
    }

    Character* character = PlayersList::instance()->getCharacter(idChar);
    m_sheetToCharacter.insert(sheet,character);

    addCharacterSheet(sheet);
    m_model.readRootSection(&msg);
    if(nullptr!=character)
    {
        character->setSheet(sheet);
    }
}

void CharacterSheetWindow::exportPDF()
{
    auto qmlView = qobject_cast<QQuickWidget*>(m_tabs->currentWidget());

    if(nullptr == qmlView)
        return;

    QObject* root = qmlView->rootObject();
    if(nullptr == root)
        return;

    auto maxPage =  QQmlProperty::read(root, "maxPage").toInt();
    auto currentPage =  QQmlProperty::read(root, "page").toInt();
    auto sheetW =  QQmlProperty::read(root, "width").toReal();
    auto sheetH =  QQmlProperty::read(root, "height").toReal();

    QObject *imagebg = root->findChild<QObject*>("imagebg");
    if (nullptr != imagebg)
    {
        sheetW =  QQmlProperty::read(imagebg, "width").toReal();
        sheetH =  QQmlProperty::read(imagebg, "height").toReal();
    }
    else
    {
        sheetW = width();
        sheetH =  height();
    }
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        QPainter painter;
        if (painter.begin(&printer))
        {
            for(int i = 0 ; i <= maxPage ; ++i)
            {
                root->setProperty("page",i);
                QTimer timer;
                timer.setSingleShot(true);
                QEventLoop loop;
                connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
                timer.start(m_preferences->value("waitingTimeBetweenPage",300).toInt());
                loop.exec();

                auto image = qmlView->grabFramebuffer();
                QRectF rect(0,0,printer.width(),printer.height());
                QRectF source(0.0, 0.0, sheetW, sheetH);
                painter.drawImage(rect,image, source);
                if(i != maxPage)
                    printer.newPage();
            }
            painter.end();
        }
    }
    root->setProperty("page",currentPage);

}
