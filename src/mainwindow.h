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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsView>
#include <QHash>
#include <QMainWindow>
#include <QPixmap>
#include <QUndoStack>

#include "canvas.h"
#include "charactersheetmodel.h"
#include "common/controller/logcontroller.h"
#include "dialog/pdfmanager.h"
#include "dialog/sheetproperties.h"
#include "field.h"
#include "fieldmodel.h"
#include "imagemodel.h"
#include "itemeditor.h"
#include "preferences/preferencesmanager.h"
#include "rolisteamimageprovider.h"

class CodeEditor;
class LogPanel;
class CharacterController;
class EditorController;
class QmlGeneratorController;
class ImageController;
namespace Ui
{
class MainWindow;
}
/**
 * @brief The MainWindow class displays all components to edit and manage the character sheet.
 */
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

    QString currentFile() const;
    void setCurrentFile(const QString& filename);

public slots:
    void openPDF();

    bool save();
    bool saveAs();

    void open();
    void openRecentFile();
    void generateQML(QString& qml);

    void showQML();
    void showQMLFromCode();

    void saveQML();
    void openQML();

    void addBackgroundImage();
    void addPage();
    void removePage();

    void openImage();
    bool mayBeSaved();
    void displayWarningsQML(const QList<QQmlError>& list);
    void aboutRcse();
    void helpOnLine();

    void exportPDF();

signals:
    void currentFileChanged();

protected:
    bool eventFilter(QObject*, QEvent*);
    bool wheelEventForView(QWheelEvent* event);
    void closeEvent(QCloseEvent* event);
    void managePDFImport();
    void defineItemCode(QModelIndex& index);
    void updateRecentFileAction();

    bool saveFile(const QString& filename);
protected slots:
    void clearData(bool addDefaultCanvas= true);
    void showPreferences();
    bool loadFile(const QString& file);

private:
    Ui::MainWindow* ui;
    std::unique_ptr<EditorController> m_editorCtrl;
    std::unique_ptr<CharacterController> m_characterCtrl;
    std::unique_ptr<QmlGeneratorController> m_qmlCtrl;
    std::unique_ptr<ImageController> m_imageCtrl;

    ItemEditor* m_view;
    EDITION_TOOL m_currentTool;
    QPoint m_startField;
    QString m_currentFile;
    bool m_qmlGeneration;
    RolisteamImageProvider* m_imgProvider;
    int m_counterZoom;
    QString m_pdfPath;
    QString m_currentUuid;
    PdfManager* m_pdf;

    // Log
    LogPanel* m_logPanel= nullptr;
    LogController* m_logManager= nullptr;

    // Recent file
    std::vector<QAction*> m_recentActions;
    QStringList m_recentFiles;
    int m_maxRecentFile= 5;
    QAction* m_separatorAction;

    QString m_title;
    PreferencesManager* m_preferences;
    QUndoStack m_undoStack;
};

#endif // MAINWINDOW_H
