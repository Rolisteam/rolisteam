#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QDockWidget>
#include <QSettings>
#include "cleveruri.h"
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

    void readSettings(QSettings & m);

    /**
     * @brief Save parameters for next executions.
     */
    void writeSettings(QSettings & m);

     CleverURI* addRessource(QString& urifile, CleverURI::ContentType type);
private:
    QTreeView* m_view;
    QHBoxLayout* m_layout;

    QWidget* m_internal;

    SessionItemModel* m_model;
    Session* m_currentSession;
};

#endif // SESSIONMANAGER_H
