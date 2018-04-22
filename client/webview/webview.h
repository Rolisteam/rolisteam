#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWidget>

namespace Ui {
class WebView;
}

class WebView : public QWidget
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = 0);
    ~WebView();

private:
    Ui::WebView *ui;
};

#endif // WEBVIEW_H
