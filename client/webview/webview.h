#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>
#include <QWebEngineView>
#include <QAction>

#include "data/mediacontainer.h"

class WebView : public MediaContainer
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = 0);
    virtual ~WebView();

    virtual bool readFileFromUri();
    virtual void saveMedia();
    virtual void putDataIntoCleverUri();

    virtual void cleverURIHasChanged(CleverURI*,CleverURI::DataValue);

protected:
    void createActions();
    void creationToolBar();

    void updateTitle();
private:
    QWebEngineView* m_view;
    QAction* m_shareAsLink;
    QAction* m_shareAsHtml;
    QAction* m_shareAsView;
    QAction* m_next;
    QAction* m_previous;
    QAction* m_reload;
    QLineEdit* m_addressEdit;

    QVBoxLayout* m_mainLayout;
};

#endif // WEBVIEW_H
