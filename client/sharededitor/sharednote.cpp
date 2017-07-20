/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "sharednote.h"

#include <QTextDocumentFragment>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <QFontDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QPrintDialog>
#include <QDebug>
#include <QCloseEvent>
#include <QDesktopWidget>
#include <QDesktopServices>
#include "ui_sharednote.h"

#include "utilities.h"
#include "enu.h"
SharedNote::SharedNote(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SharedNote)
{
    ui->setupUi(this);


    m_document = new Document(ui->m_documentSupport);
    connect(m_document->getDocument(), SIGNAL(contentsChange(int,int,int)), this, SLOT(textHasChanged(int,int,int)));
    connect(m_document->getParticipantPane(), SIGNAL(memberCanNowRead(QString)), this, SLOT(populateDocumentForUser(QString)));
    connect(m_document->getParticipantPane(), SIGNAL(memberPermissionsChanged(QString,int)), this, SLOT(playerPermissionsChanged(QString,int)));
    connect(m_document->getParticipantPane(),SIGNAL(closeMediaToPlayer(QString)),this,SLOT(closeEditorFor(QString)));


    findDialog = new FindDialog(this);
    connect(findDialog, SIGNAL(findDialogFindNext(QString,Qt::CaseSensitivity,bool,Enu::FindMode)), this,
            SLOT(findNextTriggered(QString,Qt::CaseSensitivity,bool,Enu::FindMode)));
    connect(findDialog, SIGNAL(findDialogFindPrev(QString,Qt::CaseSensitivity,bool,Enu::FindMode)), this,
            SLOT(findPrevTriggered(QString,Qt::CaseSensitivity,bool,Enu::FindMode)));
    connect(findDialog, SIGNAL(findDialogReplaceAll(QString,QString,Qt::CaseSensitivity,Enu::FindMode)), this,
            SLOT(replaceAllTriggered(QString,QString,Qt::CaseSensitivity,Enu::FindMode)));
    connect(findDialog, SIGNAL(findDialogReplace(QString)), this,
            SLOT(replaceTriggered(QString)));
    connect(findDialog, SIGNAL(findDiaalogFindReplace(QString,QString,Qt::CaseSensitivity,bool,Enu::FindMode)), this,
            SLOT(findReplaceTriggered(QString,QString,Qt::CaseSensitivity,bool,Enu::FindMode)));

    // sets the announce dialog to the state of the preferences pane
    /// @todo change name
   // announceDocumentDialog->setAnnounceDialogInfo("user", "user");

    // Connects the pref dialog to the announce dialog for when information is changed in the preferences dialog
    //connect(preferencesDialog, SIGNAL(setAnnounceDialogInfo(QString,bool)), announceDocumentDialog, SLOT(setAnnounceDialogInfo(QString,bool)));


    QGridLayout *tabLayout = new QGridLayout;
    ui->m_documentSupport->setLayout(tabLayout);
    tabLayout->addWidget(m_document);
    tabLayout->setContentsMargins(0,0,0,0);

    //ui->tab->setVisible(false);
    //ui->tabWidget->setVisible(false);

    /// @todo reset preferences stuff.
    connect(m_document, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoability(bool)));
    connect(m_document, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoability(bool)));

    readSettings();
    openPath = QDir::homePath();

    qApp->installEventFilter(this);
}

SharedNote::~SharedNote()
{
    delete ui;
}

void SharedNote::readSettings()
{
    QSettings settings("rolisteam", "SharedNote");
    QDesktopWidget *desktop = QApplication::desktop();
    int width = static_cast<int>(desktop->width() * 0.80);
    int height = static_cast<int>(desktop->height() * 0.70);
    int screenWidth = desktop->width();
    int screenHeight = desktop->height();
    QPoint pos = settings.value("pos", QPoint((screenWidth - width) / 2, (screenHeight - height) / 2)).toPoint();
    QSize size = settings.value("size", QSize(width, height)).toSize();
    resize(size);
    move(pos);
}

void SharedNote::writeSettings()
{
    QSettings settings("rolisteam", "SharedNote");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("name", "user");
}

// Protected closeEvent
void SharedNote::closeEvent(QCloseEvent *event)
{
    bool okToQuit = true;
}
void SharedNote::populateDocumentForUser(QString id)
{
    NetworkMessageWriter msg(NetMsg::SharedNoteCategory,NetMsg::updateTextAndPermission,NetworkMessage::OneOrMany);
    QStringList ids;
    ids << id;
    msg.setRecipientList(ids,NetworkMessage::OneOrMany);
    msg.string8(m_id);
    m_document->fill(&msg);
    msg.sendAll();
}

void SharedNote::setOwner(Player* player)
{
    m_document->setOwner(player);
}
bool SharedNote::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QString fileName = static_cast<QFileOpenEvent *>(event)->file(); // for the sake of mirroring code in our open slot
        if (!fileName.isEmpty())
        {



        }
        event->accept();
        return true;
    }
    return false;
}

// Save methods
bool SharedNote::save(int index)
{
    if (m_document->curFile.isEmpty()) {
        return on_actionFile_Save_As_triggered();
    }
    else {
        return saveFile(m_document->curFile);
    }
}

bool SharedNote::maybeSave(int index)
{
    if (m_document->isModified())
    {
      //  ui->tabWidget->setCurrentIndex(index);
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, "Cahoots",
                                   "The document has been modified.\n"
                                   "Do you want to save your changes?",
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save(index);
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}
void SharedNote::closeEditorFor(QString idplayer)
{
    NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::closeMedia);
    QStringList list;
    list << idplayer;
    msg.setRecipientList(list,NetworkMessage::OneOrMany);
    msg.string8(m_id);
    //msg.string8(idplayer);

    msg.sendAll();
}

bool SharedNote::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO 8859-1"));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << m_document->getPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    //statusBar()->showMessage("File saved", 4000);
    return true;
}

bool SharedNote::loadFile(const QString &fileName)
{
     QFile file(fileName);
     if (!file.open(QFile::ReadOnly | QFile::Text)) {
         QMessageBox::warning(this, tr("Application"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
         return false;
     }

     QTextStream in(&file);
     in.setCodec(QTextCodec::codecForName("ISO 8859-1"));
     QApplication::setOverrideCursor(Qt::WaitCursor);
     m_document->setPlainText(in.readAll());
     QApplication::restoreOverrideCursor();

     setCurrentFile(fileName);

     if (fileName.endsWith(".py")) {
         m_document->setHighlighter(Document::Python);
     }
     else if (fileName.endsWith(".cpp") || fileName.endsWith(".c") || fileName.endsWith(".h") || fileName.endsWith(".hpp")) {
         m_document->setHighlighter(Document::CPlusPlus);
     }
     //statusBar()->showMessage("File loaded", 4000);
     return true;
}

void SharedNote::setCurrentFile(const QString &fileName)
{
     m_document->curFile = fileName;
     m_document->setModified(false);
#warning "mechanic for showing if a document is modified"
//     setWindowModified(false); // Possible future mechanic for showing in the title bar if the document isModified

     QString shownName;
     if (m_document->curFile.isEmpty())
         shownName = "untitled.txt";
     else
         shownName = strippedName(m_document->curFile);

     //ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), shownName);
}

QString SharedNote::strippedName(const QString &fullFileName)
{
     return QFileInfo(fullFileName).fileName();
}

// File menu items
void SharedNote::on_actionFile_New_triggered()
{
 /*   int index = ui->tabWidget->addTab(new QWidget(), "untitled.txt");
    
    Document *document = new Document(ui->tabWidget->widget(index));
    QGridLayout *tabLayout = new QGridLayout;
    tabLayout->addWidget(document);
    tabLayout->setContentsMargins(0,0,0,0);
    ui->tabWidget->widget(index)->setLayout(tabLayout);

    document->setEditorFont(preferencesDialog->getEditorFont());
    document->setChatFont(preferencesDialog->getChatFont());
    document->setParticipantsFont(preferencesDialog->getParticipantsFont());

    tabWidgetToDocumentMap.insert(ui->tabWidget->widget(index), document);

    connect(document, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoability(bool)));
    connect(document, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoability(bool)));
    
    ui->tabWidget->setCurrentIndex(index);
    ui->actionTools_Announce_Document->setEnabled(true);


    ui->actionWindow_Next_Document->setEnabled(ui->tabWidget->count() > 1);
    ui->actionWindow_Previous_Document->setEnabled(ui->tabWidget->count() > 1);*/

}

void SharedNote::on_actionFile_Open_triggered()
{
 /*   QString fileName = QFileDialog::getOpenFileName(
            this,
            "Open text file",
            openPath,
            "Text files (*.*)");
    if (!fileName.isEmpty()) {
        int index = ui->tabWidget->addTab(new QWidget(), QFileInfo(fileName).fileName());

        Document *document = new Document(ui->tabWidget->widget(index));

        QGridLayout *tabLayout = new QGridLayout;
        tabLayout->addWidget(document);
        tabLayout->setContentsMargins(0,0,0,0);
        ui->tabWidget->widget(index)->setLayout(tabLayout);

        tabWidgetToDocumentMap.insert(ui->tabWidget->widget(index), document);

        connect(document, SIGNAL(undoAvailable(bool)), this, SLOT(setUndoability(bool)));
        connect(document, SIGNAL(redoAvailable(bool)), this, SLOT(setRedoability(bool)));

        document->setEditorFont(preferencesDialog->getEditorFont());
        document->setChatFont(preferencesDialog->getChatFont());
        document->setParticipantsFont(preferencesDialog->getParticipantsFont());

        ui->tabWidget->setCurrentIndex(index);

        loadFile(fileName);
        openPath = QFileInfo(fileName).path();

        ui->actionTools_Announce_Document->setEnabled(true);

        ui->actionWindow_Next_Document->setEnabled(ui->tabWidget->count() > 1);
        ui->actionWindow_Previous_Document->setEnabled(ui->tabWidget->count() > 1);
    }*/
}

bool SharedNote::on_actionFile_Save_triggered()
{
    if (m_document->curFile.isEmpty()) {
        return on_actionFile_Save_As_triggered();
    }
    else {
        return saveFile(m_document->curFile);
    }
}

bool SharedNote::on_actionFile_Save_As_triggered()
{
    qApp->processEvents(); // Redraw so we see the document we're saving along with the dialog
    // This is in case program control changes documents on a "Save All" or closeEvent
    QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save As...",
            m_document->curFile.isEmpty() ?
                QDir::homePath() + "/untitled.txt" :
                m_document->curFile,
            "Text (*.txt)");
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool SharedNote::on_actionFile_Save_A_Copy_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
            this,
            "Save A Copy As...",
            m_document->curFile.isEmpty() ?
                QDir::homePath() + "/untitled.txt" :
                m_document->curFile,
            "Text (*.txt)");
    if (fileName.isEmpty())
        return false;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    out.setCodec(QTextCodec::codecForName("ISO 8859-1"));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << m_document->getPlainText();
    QApplication::restoreOverrideCursor();

    //statusBar()->showMessage("File saved as a copy", 4000);
    return true;
}

bool SharedNote::on_actionFile_Save_All_triggered()
{
    bool isAllSaved = false;
    /*QWidget *originalWidget = ui->tabWidget->currentWidget();
    for (int i = 0; i < tabWidgetToDocumentMap.size(); i++)
    {
        ui->tabWidget->setCurrentIndex(i);
        isAllSaved += on_actionFile_Save_triggered();
    }
    ui->tabWidget->setCurrentWidget(originalWidget);*/
    return isAllSaved;
}

void SharedNote::on_actionFile_Close_triggered()
{
   /* tabCloseClicked(ui->tabWidget->currentIndex());

    ui->actionWindow_Next_Document->setEnabled(ui->tabWidget->count() > 1);
    ui->actionWindow_Previous_Document->setEnabled(ui->tabWidget->count() > 1);*/
}

void SharedNote::on_actionFile_Print_triggered()
{
    QTextDocument document(m_document->getPlainText(), this);
    document.setDefaultFont(QFont("Courier", 10, -1));

    QPrinter printer(QPrinter::HighResolution);

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle("Print");
    if (dialog->exec() == QDialog::Accepted) {
        document.print(&printer);
    }
}

// Edit menu items
void SharedNote::on_actionEdit_Undo_triggered()
{
    m_document->undo();
}

void SharedNote::on_actionEdit_Redo_triggered()
{
    m_document->redo();
}

void SharedNote::on_actionEdit_Cut_triggered()
{
    m_document->cut();
}

void SharedNote::on_actionEdit_Copy_triggered()
{
    m_document->copy();
}

void SharedNote::on_actionEdit_Paste_triggered()
{
    m_document->paste();
}

void SharedNote::on_actionEdit_Find_All_triggered()
{
    m_document->findAll();
}

void SharedNote::on_actionEdit_Find_triggered()
{
    findDialog->show();
}



void SharedNote::on_actionView_Line_Wrap_triggered()
{
    m_document->toggleLineWrap();
}

void SharedNote::on_actionView_Hide_Show_Participants_triggered()
{
    m_document->setParticipantsHidden(!m_document->isParticipantsHidden());
}
//#include "userlist/playersList.h"
void SharedNote::textHasChanged(int pos, int charsRemoved, int charsAdded)
{
    PlayersList* list = PlayersList::instance();
    Player* player = list->getLocalPlayer();
    if(m_document->canWrite(player))
    {
        QString toSend;
        QString data;

        if (charsRemoved > 0 && charsAdded == 0)
        {
            data = "";
        }
        else if (charsAdded > 0)
        {
            QTextCursor cursor = QTextCursor(m_document->getDocument());
            cursor.setPosition(pos, QTextCursor::MoveAnchor);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
            data = cursor.selection().toPlainText();
        }

        toSend = QString("doc:%1 %2 %3 %4").arg(pos).arg(charsRemoved).arg(charsAdded).arg(data);

        writeToAll(toSend);
    }
}
void SharedNote::writeToAll(QString string)
{
    qDebug() << string;
    if(!string.isEmpty())
    {
        NetworkMessageWriter msg(NetMsg::SharedNoteCategory,NetMsg::updateText);
        msg.string8(m_id);
        msg.string32(string);
        msg.sendAll();
    }
}
void SharedNote::runUpdateCmd(QString cmd)
{
    m_document->runUpdateCmd(cmd);
}

void SharedNote::displaySharingPanel()
{
    m_document->displayParticipantPanel();
}
void SharedNote::readFromMsg(NetworkMessageReader* msg)
{
    if(nullptr!=m_document)
    {
        m_document->readFromMsg(msg);
    }
}
void SharedNote::on_actionTools_Preview_as_Html_triggered()
{
    if(nullptr!=m_document)
    {
        m_document->previewAsHtml();
    }
}

void SharedNote::updateDocumentToAll(NetworkMessageWriter* msg)
{
    if(nullptr!=m_document)
    {
        m_document->fill(msg);
    }
}

void SharedNote::on_actionText_Shift_Left_triggered()
{
    m_document->shiftLeft();
}

void SharedNote::on_actionText_Shift_Right_triggered()
{
    m_document->shiftRight();
}

void SharedNote::on_actionText_Comment_Line_triggered()
{
    m_document->unCommentSelection();
}



void SharedNote::setUndoability(bool b)
{
    if (sender() == m_document) {
        ui->actionEdit_Undo->setEnabled(b);
    }
}

void SharedNote::setRedoability(bool b)
{
    if (sender() == m_document) {
        ui->actionEdit_Redo->setEnabled(b);
    }
}

void SharedNote::documentChanged(int index)
{
    Document *document = m_document;
    ui->actionEdit_Undo->setEnabled(document->isUndoable());
    ui->actionEdit_Redo->setEnabled(document->isRedoable());
    //ui->actionWindow_Split->setDisabled(document->isEditorSplit() && !document->isEditorSplitSideBySide());
   // ui->actionWindow_Split_Side_by_Side->setDisabled(document->isEditorSplit() && document->isEditorSplitSideBySide());

   // ui->actionWindow_Remove_Split->setEnabled(document->isEditorSplit());
    ui->actionTools_Announce_Document->setDisabled(document->docHasCollaborated());
}

void SharedNote::tabCloseClicked(int index)
{
 /*   if (maybeSave(index)) {
        if (ui->tabWidget->count() == 1) {
            on_actionFile_New_triggered();
        }
        ui->tabWidget->widget(index)->deleteLater();
        tabWidgetToDocumentMap.remove(ui->tabWidget->widget(index));
        ui->tabWidget->removeTab(index);
    }*/
}

void SharedNote::findNextTriggered(QString str, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    m_document->findNext(str, sensitivity, wrapAround, mode);
}

void SharedNote::findPrevTriggered(QString str, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    m_document->findPrev(str, sensitivity, wrapAround, mode);
}

void SharedNote::replaceAllTriggered(QString find, QString replace, Qt::CaseSensitivity sensitivity, Enu::FindMode mode)
{
    m_document->replaceAll(find, replace, sensitivity, mode);
}

void SharedNote::replaceTriggered(QString replace)
{
    m_document->replace(replace);
}

void SharedNote::findReplaceTriggered(QString find, QString replace, Qt::CaseSensitivity sensitivity, bool wrapAround, Enu::FindMode mode)
{
    m_document->findReplace(find, replace, sensitivity, wrapAround, mode);
}

void SharedNote::connectToDocument(QStringList list)
{

}
void SharedNote::playerPermissionsChanged(QString id,int perm)
{
    //QString toSend = QString("updateperm:%1").arg(permissions);
    NetworkMessageWriter msg(NetMsg::SharedNoteCategory,NetMsg::updatePermissionOneUser);
    msg.string8(m_id);//MediaId
    msg.string8(id);//playerId
    msg.int8(perm);//Permission
    msg.sendAll();
}


void SharedNote::setEditorFont(QFont font)
{
    m_document->setEditorFont(font);

}

void SharedNote::setChatFont(QFont font)
{
    m_document->setChatFont(font);
}

void SharedNote::setParticipantsFont(QFont font)
{
    m_document->setParticipantsFont(font);
}

QString SharedNote::id() const
{
    return m_id;
}

void SharedNote::setId(const QString &id)
{
    m_id = id;
}
