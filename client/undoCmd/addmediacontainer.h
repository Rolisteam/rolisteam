#ifndef ADDMEDIACONTENEUR_H
#define ADDMEDIACONTENEUR_H

#include <QUndoCommand>

#include "data/mediacontainer.h"
#include "session/sessionmanager.h"
#include "mainwindow.h"

class ImprovedWorkspace;

class AddMediaConteneur : public QUndoCommand
{
public:
    AddMediaConteneur(MediaContainer* mediac,SessionManager* manager,QMenu* menu, MainWindow* main, ImprovedWorkspace* workspace, bool isGM, QUndoCommand* parent = nullptr);

    void redo() override;
    void undo() override;

    bool sendAtOpening();
private:
    MediaContainer* m_media = nullptr;
    SessionManager* m_manager = nullptr;
    QMenu* m_menu = nullptr;
    MainWindow* m_main = nullptr;
    ImprovedWorkspace* m_mdiArea = nullptr;
    bool m_gm;
};

#endif // ADDMEDIACONTENEUR_H
