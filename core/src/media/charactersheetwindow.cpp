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
#include "charactersheetwindow.h"
#include "ui_charactersheetwindow.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QFile>
#include <QFileDialog>
#include <QImage>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QPrintDialog>
#include <QPrinter>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QToolButton>

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetmodel.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "data/character.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "preferences/preferencesmanager.h"
#include "sheetwidget.h"
#include "userlist/playermodel.h"

CharacterSheetWindow::CharacterSheetWindow(CharacterSheetController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::VMapContainer, parent)
    , m_sheetCtrl(ctrl)
    , m_ui(new Ui::CharacterSheetWindow)
{
    auto wid= new QWidget(this);
    m_ui->setupUi(wid);
    setWidget(wid);

    setObjectName("CharacterSheetViewer");
    connect(m_sheetCtrl, &CharacterSheetController::sheetCreated, this, &CharacterSheetWindow::addTabWithSheetView);
    /*connect(&m_model, SIGNAL(characterSheetHasBeenAdded(CharacterSheet*)), this,
            SLOT(addTabWithSheetView(CharacterSheet*)));*/

    setWindowIcon(QIcon::fromTheme("treeview"));

    m_ui->m_treeview->setModel(m_sheetCtrl->model());

    resize(m_preferences->value("charactersheetwindows/width", 400).toInt(),
           m_preferences->value("charactersheetwindows/height", 600).toInt());
    updateTitle();

    m_ui->m_treeview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->m_treeview, &QTreeView::customContextMenuRequested, this, &CharacterSheetWindow::displayCustomMenu);
    connect(m_ui->m_copyTab, &QAction::triggered, this, &CharacterSheetWindow::copyTab);
    connect(m_ui->m_readOnlyAct, &QAction::triggered, this, &CharacterSheetWindow::setReadOnlyOnSelection);
    connect(m_ui->m_addLine, &QAction::triggered, this, &CharacterSheetWindow::addLine);
    connect(m_ui->m_addSection, &QAction::triggered, this, &CharacterSheetWindow::addSection);
    connect(m_ui->m_addCharactersheet, &QAction::triggered, this, &CharacterSheetWindow::addCharacterSheet);
    connect(m_ui->m_loadQml, &QAction::triggered, this, &CharacterSheetWindow::openQML);
    connect(m_ui->m_printAct, &QAction::triggered, this, &CharacterSheetWindow::exportPDF);
    connect(m_ui->m_detachTab, &QAction::triggered, this, &CharacterSheetWindow::detachTab);
    connect(m_ui->m_stopSharingTabAct, &QAction::triggered, this, &CharacterSheetWindow::stopSharing);

    auto button= new QToolButton(this); // tr("Actions")
    button->setDefaultAction(m_ui->m_menuAct);
    m_ui->m_tabwidget->setCornerWidget(button);
    button->setEnabled(false);
    connect(m_sheetCtrl, &CharacterSheetController::cornerEnabledChanged, button, &QToolButton::setEnabled);
    connect(button, &QPushButton::clicked, this,
            [button, this]() { contextMenuForTabs(QPoint(button->pos().x(), 0)); });
}
CharacterSheetWindow::~CharacterSheetWindow() {}
void CharacterSheetWindow::addLine()
{
    // m_model.addLine(m_view.currentIndex());
}
void CharacterSheetWindow::setReadOnlyOnSelection()
{
    QList<QModelIndex> list= m_ui->m_treeview->selectionModel()->selectedIndexes();
    bool allTheSame= true;
    int i= 0;
    bool firstStatus= true;
    QList<CharacterSheetItem*> listItem;
    for(auto item : list)
    {
        if(0 == item.column())
        {
            CharacterSheetItem* csitem= static_cast<CharacterSheetItem*>(item.internalPointer());
            if(nullptr != csitem)
            {
                listItem.append(csitem);
            }
        }
        else
        {
            /* CharacterSheet* sheet= m_model.getCharacterSheet(item.column() - 1);
               if(nullptr != sheet)
               {
                   CharacterSheetItem* csitem= static_cast<CharacterSheetItem*>(item.internalPointer());
                   if(nullptr != csitem)
                   {
                       listItem.append(sheet->getFieldFromKey(csitem->getId()));
                   }
               }*/
        }
    }
    for(CharacterSheetItem* csitem : listItem)
    {
        if(nullptr != csitem)
        {
            if(i == 0)
            {
                firstStatus= csitem->isReadOnly();
            }
            if(firstStatus != csitem->isReadOnly())
            {
                allTheSame= false;
            }
        }
        ++i;
    }
    bool valueToSet;
    if(allTheSame)
    {
        valueToSet= !firstStatus;
    }
    else
    {
        valueToSet= true;
    }
    for(CharacterSheetItem* csitem : listItem)
    {
        if(nullptr != csitem)
        {
            csitem->setReadOnly(valueToSet);
        }
    }
}

void CharacterSheetWindow::updateTitle()
{
    setWindowTitle(tr("%1 - (Character Sheet Viewer)").arg(m_sheetCtrl->name()));
}

void CharacterSheetWindow::displayCustomMenu(const QPoint& pos)
{
    QMenu menu(this);

    QModelIndex index= m_ui->m_treeview->indexAt(pos);
    bool isReadOnly= false;
    if(index.column() > 0)
    {
        QMenu* affect= menu.addMenu(m_shareTo);
        addSharingMenu(affect, index.column() - 1);

        CharacterSheetItem* childItem= static_cast<CharacterSheetItem*>(index.internalPointer());
        if(nullptr != childItem)
        {
            isReadOnly= childItem->isReadOnly();
        }

        // childItem->();
    }

    menu.addSeparator();
    menu.addAction(m_ui->m_readOnlyAct);
    m_ui->m_readOnlyAct->setChecked(isReadOnly);
    /*menu.addSeparator();
    menu.addAction(m_addLine);
    menu.addAction(m_addSection);
    menu.addAction(m_addCharacterSheet);
    menu.addSeparator();
    menu.addAction(m_loadQml);*/

    addActionToMenu(menu);
    menu.exec(QCursor::pos());
}
void CharacterSheetWindow::addSharingMenu(QMenu* share, int idx)
{
    auto model= m_sheetCtrl->characterModel();
    bool hasCharacter= false;
    for(int i= 0; i < model->rowCount(); ++i)
    {
        hasCharacter= true;
        auto index= model->index(i, 0, QModelIndex());
        auto uuid= index.data(PlayerModel::IdentifierRole).toString();
        auto avatar= index.data(PlayerModel::AvatarRole).value<QImage>();
        auto name= index.data(PlayerModel::NameRole).toString();
        QAction* action= share->addAction(QPixmap::fromImage(avatar), name);
        action->setData(uuid);
        connect(action, &QAction::triggered, this,
                [this, uuid, idx]() { m_sheetCtrl->shareCharacterSheetTo(uuid, idx); });
    }

    if(hasCharacter)
    {
        share->addSeparator();
        auto act= share->addAction(tr("To all"));
        connect(act, &QAction::triggered, this, [this, idx]() { m_sheetCtrl->shareCharacterSheetToAll(idx); });
    }
}

void CharacterSheetWindow::contextMenuForTabs(const QPoint pos)
{
    QMenu menu(this);

    QWidget* wid= m_ui->m_tabwidget->currentWidget();
    SheetWidget* quickWid= dynamic_cast<SheetWidget*>(wid);
    m_currentCharacterSheet= quickWid->sheet();

    if(nullptr == m_currentCharacterSheet)
        return;

    menu.addAction(m_ui->m_detachTab);
    QMenu* share= menu.addMenu(m_shareTo);

    menu.addAction(m_ui->m_copyTab);
    menu.addAction(m_ui->m_stopSharingTabAct);
    menu.addSeparator();
    addSharingMenu(share, m_ui->m_tabwidget->currentIndex() - 1);
    addActionToMenu(menu);
    menu.addAction(m_ui->m_printAct);

    menu.exec(quickWid->mapToGlobal(pos));
}
bool CharacterSheetWindow::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::Hide)
    {
        SheetWidget* wid= dynamic_cast<SheetWidget*>(object);
        if(nullptr != wid)
        {
            if(-1 == m_ui->m_tabwidget->indexOf(wid))
            {
                wid->removeEventFilter(this);
                m_ui->m_tabwidget->addTab(wid, wid->windowTitle());
            }
        }
        return MediaContainer::eventFilter(object, event);
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
    return MediaContainer::eventFilter(object, event);
}

void CharacterSheetWindow::stopSharing()
{
    // SheetWidget* wid= dynamic_cast<SheetWidget*>(m_ui->m_tabwidget->currentWidget());
    // if(nullptr != wid)
    {
        // CharacterSheet* sheet= wid->sheet();
        /*        if(m_sheetToPerson.contains(sheet))
                {
                    Player* currentPlayer= m_sheetToPerson.value(sheet);
                    if(nullptr == currentPlayer)
                        return;

                    NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::closeCharacterSheet);
                    QStringList recipiants;
                    recipiants << currentPlayer->getUuid();
                    msg.setRecipientList(recipiants, NetworkMessage::OneOrMany);
                    msg.string8(m_mediaId);
                    msg.string8(sheet->getUuid());
                    / *PlayerModel* list= PlayerModel::instance();
                    if(list->hasPlayer(currentPlayer))
                    {
                        msg.sendToServer();
                    }* /
                    m_sheetToPerson.remove(sheet);
                }*/
    }
}

void CharacterSheetWindow::detachTab()
{
    QWidget* wid= m_ui->m_tabwidget->currentWidget();
    QString title= m_ui->m_tabwidget->tabBar()->tabText(m_ui->m_tabwidget->currentIndex());
    m_ui->m_tabwidget->removeTab(m_ui->m_tabwidget->currentIndex());
    wid->installEventFilter(this);
    emit addWidgetToMdiArea(wid, title);
}
void CharacterSheetWindow::copyTab()
{
    SheetWidget* wid= dynamic_cast<SheetWidget*>(m_ui->m_tabwidget->currentWidget());
    if(nullptr != wid)
    {
        CharacterSheet* sheet= wid->sheet();
        if(nullptr != sheet)
        {
            // addTabWithSheetView(sheet);
        }
    }
}

void CharacterSheetWindow::affectSheetToCharacter(const QString& uuid)
{
    Q_UNUSED(uuid)

    /*Character* character= PlayerModel::instance()->getCharacter(key);
    if(nullptr != character)
    {
        CharacterSheet* sheet= m_currentCharacterSheet;
        if(sheet == nullptr)
            return;

        SheetWidget* quickWid= nullptr;
        for(int i= 0, total= m_ui->m_tabwidget->count(); i < total; ++i)
        {
            auto wid= dynamic_cast<SheetWidget*>(m_ui->m_tabwidget->widget(i));
            if(wid != nullptr && sheet == wid->sheet())
            {
                quickWid= wid;
            }
        }

        checkAlreadyShare(sheet);
        m_sheetToCharacter.insert(sheet, character);
        character->setSheet(sheet);
        addTabWithSheetView(sheet);
        sheet->setName(character->name());
        m_ui->m_tabwidget->setTabText(m_ui->m_tabwidget->indexOf(quickWid), sheet->getName());

        Player* parent= character->getParentPlayer();
        Player* localItem= PlayerModel::instance()->getLocalPlayer();
        if((nullptr != parent) && (nullptr != localItem) && (localItem->isGM()))
        {
            m_sheetToPerson.insert(sheet, parent);

            Player* person= character->getParentPlayer();
            if(nullptr != person)
            {
                NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::addCharacterSheet);
                QStringList idList;
                idList << person->getUuid();
                msg.setRecipientList(idList, NetworkMessage::OneOrMany);
                msg.string8(parent->getUuid());
                fillMessage(&msg, sheet, character->getUuid());
                msg.sendToServer();
            }
        }
    }*/
}
void CharacterSheetWindow::checkAlreadyShare(CharacterSheet* sheet)
{
    Q_UNUSED(sheet)
    /* if(m_sheetToPerson.contains(sheet))
     {
         Player* olderParent= m_sheetToPerson.value(sheet);
         if(nullptr != olderParent)
         {
             NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::closeCharacterSheet);
             QStringList recipiants;
             recipiants << olderParent->getUuid();
             msg.setRecipientList(recipiants, NetworkMessage::OneOrMany);
             msg.string8(m_mediaId);
             msg.string8(sheet->getUuid());
             / *PlayerModel* list= PlayerModel::instance();
             if(list->hasPlayer(olderParent))
             {
                 msg.sendToServer();
             }* /
         }
         m_sheetToPerson.remove(sheet);
     }*/
}
bool CharacterSheetWindow::hasCharacterSheet(QString id)
{
    Q_UNUSED(id)
    /*if(nullptr == m_model.getCharacterSheetById(id))
    {
        return false;
    }
    else
    {
        return true;
    }*/
    return false;
}

void CharacterSheetWindow::removeConnection(Player* player)
{
    Q_UNUSED(player)
    /*  CharacterSheet* key= m_sheetToPerson.key(player, nullptr);
      if(nullptr != key)
      {
          m_sheetToPerson.remove(key);
      }*/
}

void CharacterSheetWindow::addSection()
{
    // m_model.addSection();
}
void CharacterSheetWindow::addCharacterSheet()
{
    /* m_errorList.clear();
     m_model.addCharacterSheet();
     displayError(m_errorList);*/
}
void CharacterSheetWindow::addTabWithSheetView(CharacterSheet* chSheet, Character* character)
{
    if(chSheet == nullptr || nullptr == character)
        return;

    auto qmlView= new SheetWidget(this);
    connect(qmlView, &SheetWidget::customMenuRequested, this, &CharacterSheetWindow::contextMenuForTabs);
    auto imageProvider= new RolisteamImageProvider(m_sheetCtrl->imageModel());
    auto engineQml= qmlView->engine();

    engineQml->addImageProvider(QLatin1String("rcs"), imageProvider);
    engineQml->addImportPath(QStringLiteral("qrc:/charactersheet/qml"));
    engineQml->rootContext()->setContextProperty("_character", character);

    for(int i= 0; i < chSheet->getFieldCount(); ++i)
    {
        CharacterSheetItem* field= chSheet->getFieldAt(i);
        if(nullptr != field)
        {
            qmlView->engine()->rootContext()->setContextProperty(field->getId(), field);
        }
    }

    QTemporaryFile fileTemp;

    if(fileTemp.open()) // QIODevice::WriteOnly
    {
        fileTemp.write(m_sheetCtrl->qmlCode().toUtf8());
        fileTemp.close();
    }

    qmlView->setSource(QUrl::fromLocalFile(fileTemp.fileName())); // WARNING take time
    qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    readErrorFromQML(qmlView->errors());
    m_errorList.append(qmlView->errors());

    QObject* root= qmlView->rootObject();

    // CONNECTION TO SIGNAL FROM QML CHARACTERSHEET
    connect(root, SIGNAL(showText(QString)), m_sheetCtrl, SIGNAL(showText(QString)));
    connect(root, SIGNAL(rollDiceCmd(QString, bool)), m_sheetCtrl, SLOT(rollDice(QString, bool)));
    connect(root, SIGNAL(rollDiceCmd(QString)), m_sheetCtrl, SLOT(rollDice(QString)));

    qmlView->setSheet(chSheet);
    int id= m_ui->m_tabwidget->addTab(qmlView, chSheet->getTitle());
    if(!m_sheetCtrl->localGM())
    {
        m_ui->m_tabwidget->setCurrentIndex(id);
    }
}
void CharacterSheetWindow::readErrorFromQML(QList<QQmlError> list)
{
    for(auto& error : list)
    {
        emit errorOccurs(error.toString());
    }
}

void CharacterSheetWindow::rollDice(QString str, bool alias)
{
    QObject* obj= sender();
    QString label;
    for(int i= 0, total= m_ui->m_tabwidget->count(); i < total; ++i)
    {
        SheetWidget* qmlView= dynamic_cast<SheetWidget*>(m_ui->m_tabwidget->widget(i));

        if(nullptr == qmlView)
            continue;

        QObject* root= qmlView->rootObject();
        if(root == obj)
        {
            label= m_ui->m_tabwidget->tabText(m_ui->m_tabwidget->indexOf(qmlView));
        }
    }
    emit rollDiceCmd(str, label, alias);
}

void CharacterSheetWindow::processUpdateFieldMessage(NetworkMessageReader* msg, const QString& idSheet)
{
    Q_UNUSED(msg)
    Q_UNUSED(idSheet)
    /*CharacterSheet* currentSheet= m_model.getCharacterSheetById(idSheet);

    if(nullptr == currentSheet)
        return;

    auto path= msg->string32();
    QByteArray array= msg->byteArray32();
    if(array.isEmpty())
        return;

    QJsonDocument doc= QJsonDocument::fromBinaryData(array);
    QJsonObject obj= doc.object();
    currentSheet->setFieldData(obj, path);*/
}
void CharacterSheetWindow::displayError(const QList<QQmlError>& warnings)
{
    QString result= "";
    for(auto& error : warnings)
    {
        result+= error.toString();
    }
    if(!warnings.isEmpty())
    {
        QMessageBox::information(this, tr("QML Errors"), result, QMessageBox::Ok);
    }
}

QJsonDocument CharacterSheetWindow::saveFile()
{

    /*if(nullptr == m_uri)
    {
        qWarning() << "no uri for charactersheet";
        return {};
    }

    QJsonDocument json;
    QJsonObject obj;
    auto path= m_uri->getUri();

    QByteArray data;
    if(path.isEmpty())
    {
        data= m_uri->getData();
    }
    else
    {
        QFile file(path);
        if(file.open(QIODevice::ReadOnly))
        {
            data= file.readAll();
            file.close();
        }
    }

    if(data.isEmpty())
    {
        qWarning() << "Charactersheet with empty data";
        return {};
    }

    json= QJsonDocument::fromJson(data);
    obj= json.object();

    // Get datamodel
    obj["data"]= m_data;

    // qml file
    // obj["qml"]=m_qmlData;*/

    // background
    /*QJsonArray images = obj["background"].toArray();
    for(auto key : m_pixmapList->keys())
    {
        QJsonObject obj;
        obj["key"]=key;
        obj["isBg"]=;
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
    obj["background"]=images;*/
    /* m_model.writeModel(obj, false);
     json.setObject(obj);
     return json;*/
    return {};
}

void CharacterSheetWindow::openQML()
{
    m_qmlUri= QFileDialog::getOpenFileName(this, tr("Open Character Sheets View"),
                                           m_preferences->value(QString("DataDirectory"), QVariant(".")).toString(),
                                           tr("Character Sheet files (*.qml)"));
    if(!m_qmlUri.isEmpty())
    {
        QFile file(m_qmlUri);
        if(file.open(QIODevice::ReadOnly))
        {
            m_sheetCtrl->setQmlCode(QString(file.readAll()));
        }
    }
}

void CharacterSheetWindow::closeEvent(QCloseEvent* event)
{
    setVisible(false);
    event->ignore();
}

void CharacterSheetWindow::addCharacterSheetSlot(CharacterSheet* sheet)
{
    if(nullptr != sheet)
    {
        // connect(sheet, &CharacterSheet::updateField, this, &CharacterSheetWindow::updateFieldFrom);
        //       m_model.addCharacterSheet(sheet, false);
    }
}

bool CharacterSheetWindow::readFileFromUri()
{
    /*  if(nullptr != m_uri)
      {
          updateTitle();

          if(m_uri->getCurrentMode() == CleverURI::Internal || !m_uri->exists())
          {
              QByteArray data= m_uri->getData();
              m_uri->setCurrentMode(CleverURI::Internal);
              QJsonDocument doc= QJsonDocument::fromBinaryData(data);
              return readData(doc.toJson());
          }
          else if(m_uri->getCurrentMode() == CleverURI::Linked)
          {
              return openFile(m_uri->getUri());
          }
      }*/
    return false;
}
void CharacterSheetWindow::putDataIntoCleverUri()
{
    /*QByteArray data;
    QJsonDocument doc= saveFile();
    data= doc.toBinaryData();*/
    /* if(nullptr != m_uri)
     {
         m_uri->setData(data);
     }*/
}

void CharacterSheetWindow::saveMedia()
{
    /*if((nullptr != m_uri) && (!m_uri->getUri().isEmpty()))
    {
        QString uri= m_uri->getUri();
        if(!uri.isEmpty())
        {
            if(!uri.endsWith(".rcs"))
            {
                uri+= QLatin1String(".rcs");
            }
            QJsonDocument doc= saveFile();
            QFile file(uri);
            if(file.open(QIODevice::WriteOnly))
            {
                file.write(doc.toJson());
                file.close();
            }
        }
    }*/
}
void CharacterSheetWindow::fillMessage(NetworkMessageWriter* msg, CharacterSheet* sheet, QString idChar)
{
    Q_UNUSED(msg)
    Q_UNUSED(sheet)
    Q_UNUSED(idChar)
    /*msg->string8(m_mediaId);
    msg->string8(idChar);
    msg->string8(getUriName());

    if(nullptr != sheet)
    {
        sheet->fill(*msg);
    }
    msg->string32(m_qmlData);*/
    //   m_imageModel->fill(*msg);

    //  m_model.fillRootSection(msg);
}

void CharacterSheetWindow::readMessage(NetworkMessageReader& msg)
{
    Q_UNUSED(msg)
    /*CharacterSheet* sheet= new CharacterSheet();

    m_mediaId= msg.string8();
    QString idChar= msg.string8();
    setUriName(msg.string8());

    if(nullptr != sheet)
    {
        sheet->read(msg);
    }*/
    // m_qmlData= msg.string32();
    //  m_imageModel->read(msg);

    /*Character* character= PlayerModel::instance()->getCharacter(idChar);
    m_sheetToCharacter.insert(sheet, character);

    addCharacterSheet(sheet);
    m_model.readRootSection(&msg);
    if(nullptr != character)
    {
        character->setSheet(sheet);
    }*/
}

void CharacterSheetWindow::exportPDF()
{
    auto qmlView= qobject_cast<SheetWidget*>(m_ui->m_tabwidget->currentWidget());

    if(nullptr == qmlView)
        return;

    QObject* root= qmlView->rootObject();
    if(nullptr == root)
        return;

    auto maxPage= QQmlProperty::read(root, "maxPage").toInt();
    auto currentPage= QQmlProperty::read(root, "page").toInt();
    auto sheetW= QQmlProperty::read(root, "width").toReal();
    auto sheetH= QQmlProperty::read(root, "height").toReal();

    QObject* imagebg= root->findChild<QObject*>("imagebg");
    if(nullptr != imagebg)
    {
        sheetW= QQmlProperty::read(imagebg, "width").toReal();
        sheetH= QQmlProperty::read(imagebg, "height").toReal();
    }
    else
    {
        sheetW= width();
        sheetH= height();
    }
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if(dialog.exec() == QDialog::Accepted)
    {
        QPainter painter;
        if(painter.begin(&printer))
        {
            for(int i= 0; i <= maxPage; ++i)
            {
                root->setProperty("page", i);
                QTimer timer;
                timer.setSingleShot(true);
                QEventLoop loop;
                connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
                timer.start(m_preferences->value("waitingTimeBetweenPage", 300).toInt());
                loop.exec();

                auto image= qmlView->grabFramebuffer();
                QRectF rect(0, 0, printer.width(), printer.height());
                QRectF source(0.0, 0.0, sheetW, sheetH);
                painter.drawImage(rect, image, source);
                if(i != maxPage)
                    printer.newPage();
            }
            painter.end();
        }
    }
    root->setProperty("page", currentPage);
}
