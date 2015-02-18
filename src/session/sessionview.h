#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QTreeView>

class SessionView : public QTreeView
{
    Q_OBJECT
public:
    explicit SessionView(QWidget *parent = 0);
    QModelIndexList getSelection();

protected:
    void contextMenuEvent ( QContextMenuEvent * event );

    void mouseDoubleClickEvent( QMouseEvent * );
signals:
    void addChapter(QModelIndex&);
    void removeSelection();
    void onDoubleClick(QModelIndex&);

private slots:
    void onAddChapter();
private:
    QAction* m_addChapterAction;
    QAction* m_removeAction;

    QPoint m_pointClick;

};

#endif // SESSIONVIEW_H
