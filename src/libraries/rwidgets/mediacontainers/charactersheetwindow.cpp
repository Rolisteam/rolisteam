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
#include "charactersheet/csitem.h"
#include "charactersheet_widgets/sheetwidget.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "data/character.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "model/playermodel.h"
#include "preferences/preferencesmanager.h"

CharacterSheetWindow::CharacterSheetWindow(CharacterSheetController* ctrl, QWidget* parent)
    : MediaContainer(ctrl, MediaContainer::ContainerType::CharacterSheetContainer, parent)
    , m_sheetCtrl(ctrl)
    , m_ui(new Ui::CharacterSheetWindow)
{
    auto wid= new QWidget(this);
    m_ui->setupUi(wid);
    setWidget(wid);

    setObjectName("CharacterSheetViewer");
    connect(m_sheetCtrl, &CharacterSheetController::sheetCreated, this, &CharacterSheetWindow::addTabWithSheetView);

    setWindowIcon(QIcon::fromTheme("treeview"));

    if(m_sheetCtrl)
        m_ui->m_treeview->setModel(m_sheetCtrl->model());

    resize(m_preferences->value("charactersheetwindows/width", 400).toInt(),
           m_preferences->value("charactersheetwindows/height", 600).toInt());

    m_ui->m_treeview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->m_treeview, &QTreeView::customContextMenuRequested, this, &CharacterSheetWindow::displayCustomMenu);
    connect(m_ui->m_copyTab, &QAction::triggered, this, &CharacterSheetWindow::copyTab);
    connect(m_ui->m_readOnlyAct, &QAction::triggered, this, &CharacterSheetWindow::setReadOnlyOnSelection);
    // connect(m_ui->m_addLine, &QAction::triggered, this, &CharacterSheetWindow::addLine);
    // connect(m_ui->m_addSection, &QAction::triggered, this, &CharacterSheetWindow::addSection);
    // connect(m_ui->m_addCharactersheet, &QAction::triggered, this, &CharacterSheetWindow::addCharacterSheet);
    connect(m_ui->m_printAct, &QAction::triggered, this, &CharacterSheetWindow::exportPDF);
    connect(m_ui->m_stopSharingTabAct, &QAction::triggered, this,
            [this]()
            {
                qDebug() << "Stop Sharing:" << m_ui->m_stopSharingTabAct->data().toString();
                m_sheetCtrl->stopSharing(m_ui->m_stopSharingTabAct->data().toString());
            });

    auto button= new QToolButton(this); // tr("Actions")
    button->setDefaultAction(m_ui->m_menuAct);
    m_ui->m_tabwidget->setCornerWidget(button);
    button->setEnabled(false);
    connect(m_sheetCtrl, &CharacterSheetController::cornerEnabledChanged, button, &QToolButton::setEnabled);
    connect(button, &QPushButton::clicked, this,
            [button, this]() { contextMenuForTabs(QPoint(button->pos().x(), 0)); });

    auto updateTitle= [this]()
    {
        if(m_sheetCtrl)
        {
            auto name= m_sheetCtrl->name();
            setWindowTitle(tr("%1 - (CharacterSheet Viewer)").arg(name.isEmpty() ? tr("Unknown") : name));
        }
    };
    connect(m_sheetCtrl, &CharacterSheetController::nameChanged, this, updateTitle);
    connect(m_sheetCtrl, &CharacterSheetController::removedSheet, this,
            [this](const QString& characterSheetId, const QString& ctrlId, const QString& characterId)
            {
                Q_UNUSED(ctrlId)
                Q_UNUSED(characterId)
                auto it= std::find_if(std::begin(m_tabs), std::end(m_tabs),
                                      [characterSheetId](const QPointer<SheetWidget>& sheetWid)
                                      {
                                          auto sheet= sheetWid->sheet();
                                          if(!sheet)
                                              return false;
                                          return sheet->uuid() == characterSheetId;
                                      });

                if(it == std::end(m_tabs))
                    return;

                m_ui->m_tabwidget->removeTab(m_ui->m_tabwidget->indexOf(*it));
                m_tabs.removeAt(std::distance(std::begin(m_tabs), it));
                (*it)->deleteLater();
            });
    connect(m_sheetCtrl, &CharacterSheetController::modifiedChanged, this, updateTitle);

    updateTitle();

    if(m_sheetCtrl->remote())
    {
        auto const& set= m_sheetCtrl->sheetData();
        std::for_each(std::begin(set), std::end(set),
                      [this](const CharacterSheetInfo& data)
                      {
                          auto sheet= m_sheetCtrl->characterSheetFromId(data.m_sheetId);
                          auto characters= m_sheetCtrl->characterModel();
                          auto character= characters->character(data.m_characterId);
                          addTabWithSheetView(sheet, character);
                      });
    }
}
CharacterSheetWindow::~CharacterSheetWindow() {}

void CharacterSheetWindow::setReadOnlyOnSelection()
{
    QList<QModelIndex> list= m_ui->m_treeview->selectionModel()->selectedIndexes();
    bool allTheSame= true;
    int i= 0;
    bool firstStatus= true;
    QList<CSItem*> listItem;
    for(auto item : list)
    {
        if(0 == item.column())
        {
            CSItem* csitem= static_cast<CSItem*>(item.internalPointer());
            if(nullptr == csitem)
                continue;

            listItem.append(csitem);
        }
        else
        {
            /* CharacterSheet* sheet= m_model.getCharacterSheet(item.column() - 1);
               if(nullptr != sheet)
               {
                   TreeSheetItem* csitem= static_cast<TreeSheetItem*>(item.internalPointer());
                   if(nullptr != csitem)
                   {
                       listItem.append(sheet->getFieldFromKey(csitem->Id()));
                   }
               }*/
        }
    }
    for(auto csitem : listItem)
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
    for(auto csitem : listItem)
    {
        if(nullptr != csitem)
        {
            csitem->setReadOnly(valueToSet);
        }
    }
}

void CharacterSheetWindow::updateTitle()
{
    if(m_sheetCtrl)
        setWindowTitle(tr("%1 - (Character Sheet Viewer)").arg(m_sheetCtrl->name()));
}

void CharacterSheetWindow::displayCustomMenu(const QPoint& pos)
{
    QMenu menu(this);

    QModelIndex index= m_ui->m_treeview->indexAt(pos);
    bool isReadOnly= false;
    int idx= index.column() - 1;
    if(index.column() > 0 && m_sheetCtrl->characterCount() > 0)
    {
        QMenu* affect= menu.addMenu(tr("Share To"));
        addSharingMenu(affect, idx);

        auto childItem= dynamic_cast<CSItem*>(static_cast<TreeSheetItem*>(index.internalPointer()));
        if(nullptr != childItem)
        {
            isReadOnly= childItem->isReadOnly();
        }
    }

    if(m_sheetCtrl->alreadySharing(m_sheetCtrl->characterSheetIdFromIndex(idx)))
    {
        m_ui->m_stopSharingTabAct->setData(m_sheetCtrl->characterSheetIdFromIndex(idx));
        menu.addAction(m_ui->m_stopSharingTabAct);
        menu.addSeparator();
    }
    menu.addAction(m_ui->m_readOnlyAct);
    m_ui->m_readOnlyAct->setChecked(isReadOnly);

    addActionToMenu(menu);
    menu.exec(QCursor::pos());
}

void CharacterSheetWindow::addSharingMenu(QMenu* share, int idx)
{
    auto characters= m_sheetCtrl->characterModel();
    auto sheets= m_sheetCtrl->model();
    if(!characters || !sheets)
        return;

    auto sheet= sheets->getCharacterSheet(idx);
    if(!sheet)
        return;

    bool hasCharacter= false;
    for(int i= 0; i < characters->rowCount(); ++i)
    {
        hasCharacter= true;
        auto index= characters->index(i, 0, QModelIndex());
        auto uuid= index.data(PlayerModel::IdentifierRole).toString();
        auto avatar= index.data(PlayerModel::AvatarRole).value<QImage>();
        auto name= index.data(PlayerModel::NameRole).toString();
        QAction* action= share->addAction(QPixmap::fromImage(avatar), name);
        action->setCheckable(true);
        action->setChecked(m_sheetCtrl->alreadySharing(uuid, sheet->uuid()));
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
    // menu of the qml view
    QMenu menu(this);

    QWidget* wid= m_ui->m_tabwidget->currentWidget();
    SheetWidget* quickWid= dynamic_cast<SheetWidget*>(wid);

    if(m_sheetCtrl->localGM())
    {
        addSharingMenu(menu.addMenu(tr("Share To")), m_ui->m_tabwidget->currentIndex() - 1);
    }
    menu.addAction(m_ui->m_copyTab);

    if(m_sheetCtrl->localGM())
    {
        m_ui->m_stopSharingTabAct->setData(currentSheetId());
        menu.addAction(m_ui->m_stopSharingTabAct);
    }
    menu.addSeparator();

    addActionToMenu(menu);
    menu.addAction(m_ui->m_printAct);

    menu.exec(quickWid->mapToGlobal(pos));
}

QString CharacterSheetWindow::currentSheetId() const
{
    auto current= m_ui->m_tabwidget->currentIndex() - 1;
    if(m_tabs.empty() || current >= m_tabs.size())
        return {};

    auto wid= m_tabs.at(current);
    if(!wid)
        return {};

    auto sheet= wid->sheet();
    if(sheet)
        return sheet->uuid();
    return {};
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

void CharacterSheetWindow::copyTab()
{
    SheetWidget* wid= dynamic_cast<SheetWidget*>(m_ui->m_tabwidget->currentWidget());
    if(nullptr != wid)
    {
        CharacterSheet* sheet= wid->sheet();
        if(nullptr != sheet)
        {
            // addTabWithSheetView(sheet);
            emit addWidgetToMdiArea(wid, m_ui->m_tabwidget->tabText(m_ui->m_tabwidget->currentIndex()));
        }
    }
}

void CharacterSheetWindow::addTabWithSheetView(CharacterSheet* chSheet, Character* character)
{
    if(chSheet == nullptr)
        return;

    auto qmlView= new SheetWidget(chSheet, m_sheetCtrl->imageModel(), this);
    connect(qmlView, &SheetWidget::customMenuRequested, this, &CharacterSheetWindow::contextMenuForTabs);
    m_tabs.append(qmlView);
    auto engineQml= qmlView->engine();
    if(character == nullptr)
        engineQml->rootContext()->setContextProperty("_character", character);
    else
        engineQml->rootContext()->setContextProperty("_character", new Character); // share to all

    QTemporaryFile fileTemp;
#ifdef QT_DEBUG
    fileTemp.setAutoRemove(false);
#endif
    if(fileTemp.open()) // QIODevice::WriteOnly
    {
        fileTemp.write(m_sheetCtrl->qmlCode().toUtf8());
        fileTemp.close();
    }

    qmlView->setSource(QUrl::fromLocalFile(fileTemp.fileName())); // WARNING take time
    qmlView->setResizeMode(QQuickWidget::SizeRootObjectToView);
    readErrorFromQML(qmlView->errors());

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
        qDebug() << error.toString();
    }
}

void CharacterSheetWindow::closeEvent(QCloseEvent* event)
{
    setVisible(false);
    event->ignore();
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
