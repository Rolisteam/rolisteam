#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QTreeView>
/**
  * @brief SessionView is a QTreeView which displays all resources loaded by the user
  *
  */
class SessionView : public QTreeView
{
    Q_OBJECT
public:
    /**
      * @brief default constructor
      *
      */
    explicit SessionView(QWidget *parent = 0);
    /**
      * @brief accessor to the selected items
      * @return list of selected item index
      */
    QModelIndexList getSelection();

protected:
    /**
      * @brief catches the contextMenu event and shows a menu
      *
      */
    void contextMenuEvent ( QContextMenuEvent * event );
    /**
      * @brief catches the double Click event and open the selected item
      *
      */
    void mouseDoubleClickEvent( QMouseEvent * );
signals:
    /**
      * @brief signal to notify one chapter has been added
      * @param location in the model of the new chapter
      */
    void addChapter(QModelIndex&);
    /**
      * @brief signal to notify the selection must be deleted
      *
      */
    void removeSelection();
    /**
      * @brief signal to notify the double click on item
      *
      */
    void onDoubleClick(QModelIndex&);
    /**
      * @brief signal to notify the change of current chapter
      *
      */
    void defineCurrentChapter();
private slots:
    void onDefineChapter();
private slots:
    void onAddChapter();
private:
    QAction* m_addChapterAction;
    QAction* m_removeAction;
    QAction* m_defineAsCurrent;

    QPoint m_pointClick;

};

#endif // SESSIONVIEW_H
