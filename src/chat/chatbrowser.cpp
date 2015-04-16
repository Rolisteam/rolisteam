#include "chatbrowser.h"
#include <QColorDialog>
#include <QMenu>

#define MAX_COLOR_CHANNEL 255

ChatBrowser::ChatBrowser(QWidget *parent) :
    QTextBrowser(parent),m_bgColor(MAX_COLOR_CHANNEL,MAX_COLOR_CHANNEL,MAX_COLOR_CHANNEL)
{
    //setOpenLinks(true);
    m_bgColorAct= new QAction(tr("Background Color"),this); //toolBar->addWidget(m_bgColorSelector);
    m_bgColorAct->setToolTip(tr("Background Color"));
    connect(m_bgColorAct,SIGNAL(triggered()),this, SLOT(backGroundChanged()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));
}
void ChatBrowser::backGroundChanged()
{
    QColorDialog dialog;
    dialog.setCurrentColor(m_bgColor);
    if(dialog.exec()==QDialog::Accepted)
    {
        m_bgColor=dialog.currentColor();
        setStyleSheet(QString("background:rgb(%1,%2,%3);").arg(m_bgColor.red()).arg(m_bgColor.green()).arg(m_bgColor.blue()));
    }
}
void ChatBrowser::showContextMenu(QPoint pos)
{
    QMenu* menu = createStandardContextMenu(pos);


    menu->addAction(m_bgColorAct);


    menu->exec(mapToGlobal(pos));
}
