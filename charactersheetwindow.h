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
#include <QTreeView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHBoxLayout>
#include <QQmlEngine>
#include <QQuickWidget>

#include "data/mediacontainer.h"
#include "charactersheetmodel.h"
#include "rolisteamimageprovider.h"
/**
    * @brief herits from SubMdiWindows. It displays and manages all classes required to deal with the character sheet MVC architrecture.
    */
class CharacterSheetWindow : public MediaContainer
{
    Q_OBJECT
public:
    /**
    * @brief default construtor
    */
    CharacterSheetWindow(CleverURI* uri= NULL, QWidget* parent = NULL);
    virtual ~CharacterSheetWindow(); 
    
    bool openFile(const QString& file);
    void saveFile(const QString & file);
    virtual bool hasDockWidget() const ;
    virtual QDockWidget* getDockWidget() ;

    bool readFileFromUri();
    void saveMedia();
    
signals:
    void addWidgetToMdiArea(QWidget*);

public slots:
    void openQML();
    void contextMenuForTabs(const QPoint &pos);
    void detachTab();


protected slots:
    void addTabWithSheetView(CharacterSheet *chSheet);
    /**
    * @brief slot is called when the user click on the m_addLine button. That leads to add one line after the current position (in the current CharacterSheet).
    */
    void addLine();
    /**
    * @brief slot is called when the user click on the m_addSection button. That leads to add a section after the current section (in the current CharacterSheet).
    */
    void addSection();
    /**
    * @brief slot is called when the user click on the m_addCharacterSheet button. That Leads to add an empty characterSheet in the model (after all others).
    */
    void addCharacterSheet();
    /**
    * @brief slot is called when the user click on the  mice right button
    */
    void displayCustomMenu(const QPoint & pos);
    
    void openCharacterSheet();
    
    void saveCharacterSheet();

    void affectSheetToCharacter();
    void displayError(const QList<QQmlError> &warnings);


protected:
    virtual void closeEvent ( QCloseEvent * event );

private:
    /**
    * @brief The view class
    */
    QTreeView m_view;
    /**
    * @brief the model for the characterSheet view
    */
    CharacterSheetModel m_model;
    
    
    /**
    * @brief event button
    * @todo It should be a better idea to use QAction instead of Button and profide a contextual menu or a custom menu instead of these awful buttons.
    */
    QAction* m_addSection;
    /**
    * @brief event button
    */
    QAction* m_addLine;
    /**
    * @brief event button
    */
    QAction* m_addCharacterSheet;
    /**
    * @brief event button
    */
    QAction* m_saveCharacterSheet;
    /**
    * @brief event button
    */
    QAction* m_openCharacterSheet;

    QAction* m_loadQml;
    QAction* m_detachTab;
    /**
    * @brief layout
    */
    QVBoxLayout m_vertiLayout;
    /**
     * @brief m_fileUri
     */
    QString m_fileUri;
    QString m_qmlUri;
    /**
     * @brief m_tabs
     */
    QTabWidget* m_tabs;

    QList<QQuickWidget*> m_characterSheetlist;
    int m_currentCharacterSheet;
    RolisteamImageProvider* m_imgProvider;

    QString m_qmlData;
};

#endif // CHARACTERSHEETWINDOW_H
