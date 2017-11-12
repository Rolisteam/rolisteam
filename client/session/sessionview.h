#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QTreeView>
#include <QDrag>
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
    void startDrag(Qt::DropActions supportedActions);
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
    /**
    * @brief slot to define the current chapter
    */
    void onDefineChapter();
    
    /**
    * @brief slot to add chapter in the current item
    */
    void onAddChapter();
private:
    /**
    * @brief action triggered for adding chapter
    */
    QAction* m_addChapterAction;
    /**
    * @brief action triggered for deleting chapter
    */
    QAction* m_removeAction;
    /**
    * @brief action to select item as current one
    */
    QAction* m_defineAsCurrent;
    /**
    * @brief stores the position of the click
    */
    QPoint m_pointClick;

    QAction* m_loadingModeColumn;
    QAction* m_displayedColumn;
    QAction* m_pathColumn;
    QAction* m_rename;
    
};

#endif // SESSIONVIEW_H
