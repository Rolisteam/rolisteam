#include "chatbrowser.h"
#include <QColorDialog>
#include <QMenu>

#include "preferences/preferencesmanager.h"

#define MAX_COLOR_CHANNEL 255

ChatBrowser::ChatBrowser(QWidget *parent) :
    QTextBrowser(parent),m_bgColor(MAX_COLOR_CHANNEL,MAX_COLOR_CHANNEL,MAX_COLOR_CHANNEL),m_parent(parent)
{
    //setOpenLinks(true);
    m_bgColorAct= new QAction(tr("Background Color"),this); //toolBar->addWidget(m_bgColorSelector);
    m_bgColorAct->setToolTip(tr("Background Color"));
    connect(m_bgColorAct,SIGNAL(triggered()),this, SLOT(backGroundChanged()));


    m_detachedDialog = new QAction(tr("Detached the view"),this);
    m_detachedDialog->setCheckable(true);
    connect(m_detachedDialog,SIGNAL(triggered()),this, SLOT(detachedView()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));

    setAcceptRichText(false);
    //setContextMenuPolicy(Qt::NoContextMenu);
    setOpenLinks(true);
    setReadOnly(true);
    setUndoRedoEnabled(false);
    setWordWrapMode(QTextOption::WrapAnywhere);
   // setLineWrapMode(QTextEdit::FixedPixelWidth);
    document()->setDefaultStyleSheet(QString(".dice {color:%1;font-weight: bold;}").arg(PreferencesManager::getInstance()->value("DiceHighlightColor",QColor(Qt::red)).value<QColor>().name()));
}
void ChatBrowser::backGroundChanged()
{
    QColorDialog dialog;
    dialog.setCurrentColor(m_bgColor);

    if(dialog.exec()==QDialog::Accepted)
    {
        m_bgColor=dialog.currentColor();
        setStyleSheet(QString("QTextBrowser { background:%1;}").arg(m_bgColor.name()));
    }
}
void ChatBrowser::showContextMenu(QPoint pos)
{
    QMenu* menu = createStandardContextMenu(pos);
    menu->addAction(m_bgColorAct);
    menu->addAction(m_detachedDialog);
    menu->exec(mapToGlobal(pos));
}
void ChatBrowser::resizeEvent(QResizeEvent *e)
{
    QTextBrowser::resizeEvent(e);
}
void ChatBrowser::detachedView()
{
    if(m_detachedDialog->isChecked())
    {
        detachView(true);
        /*setParent(NULL);
        setWindowFlags(Qt::Dialog);*/
    }
    else
    {
        detachView(false);
        //setParent(m_parent);
    }
}
