#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDockWidget>

class QHBoxLayout;
class QTreeView;
class SessionItemModel;

class SessionManager : public QDockWidget
{

public:
    SessionManager();


 private:
    QTreeView* m_view;
    QHBoxLayout* m_layout;

    QWidget* m_internal;

    SessionItemModel* m_model;
};

#endif // SESSIONMANAGER_H
