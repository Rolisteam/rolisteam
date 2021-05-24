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
#include "data/mediacontainer.h"

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
class CharacterSheetItem;
class CharacterSheetWindow : public MediaContainer
{
    Q_OBJECT
public:
    /**
     * @brief default construtor
     */
    CharacterSheetWindow(CharacterSheetController* ctrl, QWidget* parent= nullptr);
    /**
     * @brief ~CharacterSheetWindow
     */
    virtual ~CharacterSheetWindow();
    /**
     * @brief saveFile
     * @return
     */
    QJsonDocument saveFile();
    /**
     * @brief addCharacterSheet
     */
    void addCharacterSheetSlot(CharacterSheet*);

    // RolisteamImageProvider* getImgProvider() const;
    // void setImgProvider(RolisteamImageProvider* imgProvider);
    /**
     * @brief fill
     * @param msg
     * @param sheet
     * @param idChar
     */
    void fillMessage(NetworkMessageWriter* msg, CharacterSheet* sheet, QString idChar);
    /**
     * @brief read
     * @param msg
     */
    void readMessage(NetworkMessageReader& msg);
    /**
     * @brief processUpdateFieldMessage
     * @param msg
     */
    void processUpdateFieldMessage(NetworkMessageReader* msg, const QString& idSheet);
    /**
     * @brief hasCharacterSheet
     * @param id
     * @return
     */
    bool hasCharacterSheet(QString id);
signals:
    /**
     * @brief addWidgetToMdiArea
     * @param str
     */
    void addWidgetToMdiArea(QWidget*, QString str);
    /**
     * @brief rollDiceCmd
     * @param str
     * @param label
     */
    void rollDiceCmd(QString str, QString label, bool withAlias);
    /**
     * @brief showText
     * @param str
     */
    void showText(QString str);
    /**
     * @brief errorOccurs
     * @param error
     */
    void errorOccurs(QString error);

public slots:
    /**
     * @brief openQML
     */
    void openQML();
    /**
     * @brief contextMenuForTabs
     * @param pos
     */
    void contextMenuForTabs(const QPoint pos);
    /**
     * @brief detachTab
     */
    void detachTab();

    /**
     * @brief rollDice
     * @param cmd : dice command
     * @param alias : use alias if true, otherwise the command stays unmodified.
     */
    void rollDice(QString cmd, bool alias= true);
    /**
     * @brief removeConnection
     */
    void removeConnection(Player*);
    /**
     * @brief readErrorFromQML
     * @param errors
     */
    void readErrorFromQML(QList<QQmlError> errors);

    /**
     * @brief exportPDF
     */
    void exportPDF();
protected slots:
    /**
     * @brief addTabWithSheetView
     * @param chSheet
     */
    void addTabWithSheetView(CharacterSheet* chSheet, Character* character);
    /**
     * @brief slot is called when the user click on the m_addLine button. That leads to add one line after the current
     * position (in the current CharacterSheet).
     */
    void addLine();
    /**
     * @brief slot is called when the user click on the m_addSection button. That leads to add a section after the
     * current section (in the current CharacterSheet).
     */
    void addSection();
    /**
     * @brief slot is called when the user click on the m_addCharacterSheet button. That Leads to add an empty
     * characterSheet in the model (after all others).
     */
    void addCharacterSheet();
    /**
     * @brief slot is called when the user click on the  mice right button
     */
    void displayCustomMenu(const QPoint& pos);
    /**
     * @brief affectSheetToCharacter
     */
    void affectSheetToCharacter(const QString& uuid);
    /**
     * @brief displayError
     * @param warnings
     */
    void displayError(const QList<QQmlError>& warnings);
    /**
     * @brief setReadOnlyOnSelection
     */
    void setReadOnlyOnSelection();
    /**
     * @brief stopSharing
     */
    void stopSharing();
    /**
     * @brief updateTitle
     */
    virtual void updateTitle();
    /**
     * @brief copyTab
     */
    void copyTab();

protected:
    /**
     * @brief closeEvent
     * @param event
     */
    virtual void closeEvent(QCloseEvent* event);
    /**
     * @brief addSharingMenu
     * @param share
     */
    void addSharingMenu(QMenu* share, int pos);
    /**
     * @brief checkAlreadyShare
     * @param sheet
     */
    void checkAlreadyShare(CharacterSheet* sheet);
    /**
     * @brief eventFilter
     * @param object
     * @param event
     * @return
     */
    bool eventFilter(QObject* object, QEvent* event);

private:
    QPointer<CharacterSheetController> m_sheetCtrl;
    Ui::CharacterSheetWindow* m_ui;
    QString m_qmlUri;

    // QMap<SheetWidget*,CharacterSheet*> m_characterSheetlist;
    CharacterSheet* m_currentCharacterSheet;
    QQmlComponent* m_sheetComponent;

    /*QHash<CharacterSheet*, Player*> m_sheetToPerson;
    QHash<CharacterSheet*, Character*> m_sheetToCharacter;
    std::unique_ptr<ImageModel> m_imageModel;*/

    QJsonObject m_data;

    QList<QQmlError> m_errorList;

    // Translation optimisation
    const QString m_shareTo= tr("Share To");
    const QString m_filterString= tr("Character Sheet Data files (*.rcs)");
};

#endif // CHARACTERSHEETWINDOW_H
