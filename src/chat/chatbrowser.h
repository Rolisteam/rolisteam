#ifndef CHATBROWSER_H
#define CHATBROWSER_H

#include <QTextBrowser>
#include <QAction>
#include <QResizeEvent>

/**
 * @brief The ChatBrowser class is the widget in charge of showing to the user all messages from one chat.
 * It also displays dice roll results, help and error message.
 */
class ChatBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit ChatBrowser(QWidget *parent = 0);
    
signals:
    void detachView(bool);
protected:
        //void contextMenuEvent ( QContextMenuEvent * event );
    virtual void resizeEvent(QResizeEvent *e);
    
private slots:
    void backGroundChanged();
    void detachedView();
    void showContextMenu(QPoint pos);

    void setWordWrap();
    void setWrapAnyWhere();


private:
    QAction* m_bgColorAct;
    QAction* m_detachedDialog;
    QAction* m_wordWarp;
    QAction* m_anyWhereWarp;

    QColor m_bgColor;
    QWidget* m_parent;
};

#endif // CHATBROWSER_H
