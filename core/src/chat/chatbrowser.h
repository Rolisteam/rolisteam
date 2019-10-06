#ifndef CHATBROWSER_H
#define CHATBROWSER_H

#include <QAction>
#include <QResizeEvent>
#include <QTextBrowser>

/**
 * @brief The ChatBrowser class is the widget in charge of showing to the user all messages from one chat.
 * It also displays dice roll results, help and error message.
 */
class ChatBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    explicit ChatBrowser(bool showTimeStatus, QWidget* parent= 0);

signals:
    void detachView(bool);
    void showTimeChanged(bool);

protected:
    // void contextMenuEvent ( QContextMenuEvent * event );
    virtual void resizeEvent(QResizeEvent* e);

private slots:
    void backGroundChanged();
    void detachedView();
    void showContextMenu(QPoint pos);

    void setWordWrap();
    // void setWrapAnyWhere();

private:
    QAction* m_bgColorAct;
    QAction* m_detachedDialog;
    QAction* m_wordWarp;
    QAction* m_showTime;
    QAction* m_clearChat;

    QColor m_bgColor;
    QWidget* m_parent;
};

#endif // CHATBROWSER_H
