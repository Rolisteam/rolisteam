#ifndef DELETEMEDIACONTAINERCOMMAND_H
#define DELETEMEDIACONTAINERCOMMAND_H

#include <QUndoCommand>
#include "data/mediacontainer.h"

class SessionManager;
class MainWindow;
class ImprovedWorkspace;
class DeleteMediaContainerCommand : public QUndoCommand
{
public:
    DeleteMediaContainerCommand(MediaContainer* media,
                                SessionManager* manager,
                                QMenu* menu,
                                MainWindow* main,
                                ImprovedWorkspace* workspace,
                                bool isGM,
                                QUndoCommand* parent = nullptr);

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

#endif // DELETEMEDIACONTAINERCOMMAND_H
