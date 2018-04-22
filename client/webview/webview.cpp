#include "webview.h"
#include "ui_webview.h"

WebView::WebView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebView)
{
    ui->setupUi(this);
}

WebView::~WebView()
{
    delete ui;
}
