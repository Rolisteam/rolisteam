#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDockWidget>

class QHBoxLayout;
class QTreeView;
class SessionItemModel;
class Chapter;
class Session;
class SessionManager : public QDockWidget
{

public:
    SessionManager();

    Chapter* getCurrentChapter();
    void setCurrentSession(Session* s);
 private:
    QTreeView* m_view;
    QHBoxLayout* m_layout;

    QWidget* m_internal;

    SessionItemModel* m_model;
    Session* m_currentSession;
};

#endif // SESSIONMANAGER_H
