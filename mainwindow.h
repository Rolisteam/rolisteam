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
#include <QMainWindow>
#include <QHash>
#include <QPixmap>
#include <QUndoStack>

#include "canvas.h"
#include "fieldmodel.h"
#include "rolisteamimageprovider.h"
#include "field.h"
#include "charactersheetmodel.h"
#include "pdfmanager.h"
#include "sheetproperties.h"
#include "preferencesmanager.h"
#include "imagemodel.h"
#include "itemeditor.h"

class CodeEditor;

namespace Ui {
class MainWindow;
}
/**
 * @brief The MainWindow class displays all components to edit and manage the character sheet.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum EDITION_TOOL {ADDFIELD,SELECT,NONE};
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool qmlGeneration() const;
    void setQmlGeneration(bool qmlGeneration);

    void updatePageSelector();
    void readSettings();
    void writeSettings();
    void cleanData();
public slots:
    void openPDF();
    void setCurrentTool();

    void save();
    void saveAs();

    void open();
    void generateQML(QString& qml);

    void showQML();
    void showQMLFromCode();

    void saveQML();
    void openQML();
    void setImage();
    void rollDice(QString cmd);
    void addPage();
    void removePage();
    void currentPageChanged(int);

    void editColor(QModelIndex);
    void openImage();
    void setFitInView();
    bool mayBeSaved();
    void modelChanged();
    void displayWarningsQML(QList<QQmlError> list);
    void aboutRcse();
    void helpOnLine();
    void addImage();
    void copyPath();
protected:
    bool eventFilter(QObject *, QEvent *);
    bool wheelEventForView(QWheelEvent *event);
    void closeEvent(QCloseEvent *event);
    void managePDFImport();
    void applyValue(QModelIndex &index, bool selection);
    void applyValueOnCharacterSelection(QModelIndex &index, bool selection, bool allCharacter);
    void defineItemCode(QModelIndex &index);

protected slots:
    void menuRequested(const QPoint &pos);
    void menuRequestedForFieldModel(const QPoint &pos);
    void menuRequestedFromView(const QPoint &pos);
    void menuRequestedForImageModel(const QPoint &pos);
    void columnAdded();
    void alignOn();
    void clearData();
    void showPreferences();
    void pageCountChanged();
    void checkCharacters();
private slots:
    void codeChanged();
//private:
    // QString getFilePath(QString);
    void sameGeometry();

private:
    int pageCount();
private:
    Ui::MainWindow *ui;
    QList<Canvas*> m_canvasList;
    ItemEditor* m_view;
    EDITION_TOOL m_currentTool;
    QPoint m_startField;
    QHash<QString,QPixmap*> m_pixList;
    FieldModel* m_model;
    QString m_filename;
    bool m_qmlGeneration;
    RolisteamImageProvider* m_imgProvider;
    CharacterSheetModel* m_characterModel;
    int m_currentPage;
    bool m_editedTextByHand;
    int m_counterZoom;
    QString m_pdfPath;
    PdfManager* m_pdf;

    ImageModel* m_imageModel;

    //Action Character
    QAction* m_addCharacter;
    QAction* m_deleteCharacter;
    QAction* m_copyCharacter;
    QAction* m_defineAsTabName;
    QAction* m_applyValueOnAllCharacterLines;
    QAction* m_applyValueOnSelectedCharacterLines;
    QAction* m_applyValueOnAllCharacters;

    //Action items
    QAction* m_delItem;
    QAction* m_applyValueOnSelection;
    QAction* m_applyValueOnAllLines;
    QAction* m_defineCode;
    QAction* m_resetCode;


    //action view
    QAction* m_fitInView;
    QAction* m_alignOnY;
    QAction* m_alignOnX;
    QAction* m_sameWidth;
    QAction* m_sameHeight;
    QAction* m_dupplicate;
    QPoint m_posMenu;

    //action image model
    QAction* m_copyPath;
    QAction* m_replaceImage;
    QAction* m_removeImage;

    QString m_title;
    PreferencesManager* m_preferences;
    /// Sheet properties
    SheetProperties* m_sheetProperties;
    QString m_additionnalCode;
    QString m_additionnalImport;

    bool m_additionnalCodeTop;
    bool m_flickableSheet;
    qreal m_fixedScaleSheet;

    QUndoStack m_undoStack;
    CodeEditor* m_codeEdit;
};

#endif // MAINWINDOW_H

