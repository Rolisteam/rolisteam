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

#ifndef CHARACTERSHEETWINDOW_H
#define CHARACTERSHEETWINDOW_H
#include <QHBoxLayout>
#include <QPushButton>
#include <QQmlEngine>
#include <QTreeView>
#include <QVBoxLayout>
#include <memory>

//#include "charactersheetmodel.h"
#include "charactersheet/rolisteamimageprovider.h"
#include "data/character.h"
#include "mediacontainers/mediacontainer.h"
#include "rwidgets_global.h"
/**
 * @page characterSheet CharacterSheet System
 *
 * @section Introduction Introduction
 * The characterSheet system is dedicated to manage charactersheet.
 * It provides a way to display the charactersheet into a graphical user interface.
 * This GUI is designed in QML and to create it, rolisteam comes with Rolisteam CharacterSheet Editor.
 * The interface displays data from data model stored in rolisteam. Those data are also read by the Dice System.
 * The GUI provides different kinds of items to display data: Textfield, TextArea, TextLine, Combobox and dice button.
 * So, it is possible to send dice command from the graphical sheet.
 * It is possible to use formula in the charactersheet.
 *
 *
 */

namespace Ui
{
class CharacterSheetWindow;
}

/**
 * @brief herits from SubMdiWindows. It displays and manages all classes required to deal with the character sheet MVC
 * architrecture.
 */
class SheetWidget;
class Player;
class CharacterSheetController;
class CharacterSheet;
class TreeSheetItem;
class RWIDGET_EXPORT CharacterSheetWindow : public MediaContainer
{
    Q_OBJECT
public:
    CharacterSheetWindow(CharacterSheetController* ctrl, QWidget* parent= nullptr);
    virtual ~CharacterSheetWindow();

public slots:
    void contextMenuForTabs(const QPoint pos);
    void readErrorFromQML(QList<QQmlError> errors);
    void exportPDF();


protected slots:
    void addTabWithSheetView(CharacterSheet* chSheet, Character* character);
    void displayCustomMenu(const QPoint& pos);
    void setReadOnlyOnSelection();
    virtual void updateTitle();
    void copyTab();

protected:
    virtual void closeEvent(QCloseEvent* event);
    void addSharingMenu(QMenu* share, int pos);
    void checkAlreadyShare(CharacterSheet* sheet);
    bool eventFilter(QObject* object, QEvent* event);

private:
    QPointer<CharacterSheetController> m_sheetCtrl;
    Ui::CharacterSheetWindow* m_ui;
    QList<QPointer<SheetWidget>> m_tabs;
};

#endif // CHARACTERSHEETWINDOW_H
