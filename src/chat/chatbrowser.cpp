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


    m_detachedDialog = new QAction(tr("Detach the view"),this);
    m_detachedDialog->setCheckable(true);
    m_wordWarp = new QAction(tr("Word Wrap"),this);
    m_wordWarp->setCheckable(true);
    m_wordWarp->setChecked(true);

    connect(m_detachedDialog,SIGNAL(triggered()),this, SLOT(detachedView()));
    //connect(m_anyWhereWarp,SIGNAL(triggered()),this, SLOT(setWrapAnyWhere()));
    connect(m_wordWarp,SIGNAL(triggered()),this, SLOT(setWordWrap()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showContextMenu(QPoint)));

    setAcceptRichText(false);
    //setContextMenuPolicy(Qt::NoContextMenu);
    setOpenLinks(true);
    setReadOnly(true);
    setUndoRedoEnabled(false);
    setWordWrapMode(QTextOption::WordWrap);
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
    menu->addSeparator();
 //   menu->addAction(m_anyWhereWarp);
    menu->addAction(m_wordWarp);
    menu->exec(mapToGlobal(pos));
}
void ChatBrowser::resizeEvent(QResizeEvent *e)
{
    QTextBrowser::resizeEvent(e);
}
void ChatBrowser::setWordWrap()
{
    if(m_wordWarp->isChecked())
    {
        setWordWrapMode(QTextOption::WordWrap);
    }
    else
    {
        setWordWrapMode(QTextOption::WrapAnywhere);
    }
}



void ChatBrowser::detachedView()
{
    if(m_detachedDialog->isChecked())
    {
        detachView(true);
    }
    else
    {
        detachView(false);
    }
}
