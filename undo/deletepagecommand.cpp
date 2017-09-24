#include "addpagecommand.h"
QStringListModel* AddPageCommand::m_pagesModel = new QStringListModel();

AddPageCommand::AddPageCommand(int currentPage, QList<Canvas*>& list,Canvas::Tool tool,QUndoCommand *parent)
  : QUndoCommand(parent),m_currentPage(currentPage),m_list(list),m_currentTool(tool)
{
    m_canvas = new Canvas();
    m_canvas->setCurrentTool(m_currentTool);
    m_canvas->setCurrentPage(m_currentPage);
    setText(QObject::tr("Add Page #%1").arg(m_currentPage));
}

void AddPageCommand::undo()
{
    m_list.removeAt(m_currentPage);
    QStringList str = m_pagesModel->stringList();
    str.removeAt(m_currentPage);
    m_pagesModel->setStringList(str);
}

void AddPageCommand::redo()
{
    m_list.insert(m_currentPage,m_canvas);
    QStringList str = m_pagesModel->stringList();
    str.insert(m_currentPage,QObject::tr("Page %1").arg(m_currentPage+1));
    m_pagesModel->setStringList(str);
}

Canvas *AddPageCommand::canvas() const
{
    return m_canvas;
}

QStringListModel* AddPageCommand::getPagesModel()
{
    return m_pagesModel;
}

void AddPageCommand::setPagesModel(QStringListModel* pagesModel)
{
    m_pagesModel = pagesModel;

}
