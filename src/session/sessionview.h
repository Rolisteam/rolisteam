#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QTreeView>

class SessionView : public QTreeView
{
    Q_OBJECT
public:
    explicit SessionView(QWidget *parent = 0);
protected:
    void contextMenuEvent ( QContextMenuEvent * event );
signals:
    void addChapter();

private:
    QAction* m_addChapterAction;

};

#endif // SESSIONVIEW_H
