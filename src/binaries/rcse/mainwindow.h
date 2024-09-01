/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * http://www.rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#ifndef RCSE_MAINWINDOW_H
#define RCSE_MAINWINDOW_H

#include <QGraphicsView>
#include <QHash>
#include <QMainWindow>
#include <QPixmap>
#include <QUndoStack>

#include "canvas.h"
#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/imagemodel.h"
#include "charactersheet/rolisteamimageprovider.h"
#include "controllers/maincontroller.h"
#include "dialog/pdfmanager.h"
#include "dialog/sheetproperties.h"
#include "fieldmodel.h"
#include "itemeditor.h"
#include "preferences/rcsepreferencesmanager.h"

class CodeEditor;
class LogPanel;
namespace Ui
{
class MainWindow;
}
/**
 * @brief The MainWindow class displays all components to edit and manage the character sheet.
 */
namespace rcse
{
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum EDITION_TOOL
    {
        ADDFIELD,
        SELECT,
        NONE
    };
    explicit MainWindow(QWidget* parent= nullptr);
    ~MainWindow();

    bool qmlGeneration() const;
    void setQmlGeneration(bool qmlGeneration);

    void updatePageSelector();
    void readSettings();
    void writeSettings();
    void cleanData();

public slots:
    void openPDF();

    bool save();
    bool saveAs();

    void open();
    void openRecentFile();

    void generateAndShowQML();
    void showQMLFromCode();

    // void saveQML();
    void openQML();

    void addBackgroundImage();
    void addPage();
    void removePage();

    void openBackgroundImage();
    void openImage();
    bool mayBeSaved();
    void helpOnLine();

    void exportPDF();
    bool loadFile(const QString& file);

protected:
    bool eventFilter(QObject*, QEvent*);
    bool wheelEventForView(QWheelEvent* event);
    void closeEvent(QCloseEvent* event);
    void defineItemCode(QModelIndex& index);
    void updateRecentFileAction();

    bool saveFile(const QString& filename);

    void setUpActionForImageTab();
    void setUpActionForCharacterTab();
protected slots:
    void showPreferences();
    void showContextMenuForImageTab();
    void showContextMenuForCharacterTab();
    void updateTitle();

private:
    Ui::MainWindow* ui{nullptr};
    std::unique_ptr<rcse::MainController> m_mainCtrl;
    EDITION_TOOL m_currentTool;
    QPoint m_startField;
    bool m_qmlGeneration;
    RolisteamImageProvider* m_imgProvider;
    int m_counterZoom;
    QString m_pdfPath;

    // Log
    LogPanel* m_logPanel= nullptr;

    // Image tab action
    QAction* m_copyPath= nullptr;
    QAction* m_copyUrl= nullptr;
    QAction* m_replaceImage= nullptr;
    QAction* m_removeImage= nullptr;
    QAction* m_reloadImageFromFile= nullptr;

    // Action for CharacterTab
    QAction* m_addCharacter= nullptr;
    QAction* m_deleteCharacter= nullptr;
    QAction* m_copyCharacter= nullptr;
    QAction* m_defineAsTabName= nullptr;
    QAction* m_applyValueOnSelectedCharacterLines= nullptr;
    QAction* m_applyValueOnAllCharacters= nullptr;

    // Recent file
    std::vector<QAction*> m_recentActions;
    QStringList m_recentFiles;
    int m_maxRecentFile= 5;
    QAction* m_separatorAction;

    PreferencesManager* m_preferences;
};
}
#endif // RCSE_MAINWINDOW_H
