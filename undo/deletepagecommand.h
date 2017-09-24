#ifndef ADDPAGECOMMAND_H
#define ADDPAGECOMMAND_H

#include <QUndoCommand>
#include "canvas.h"
#include <QObject>
#include <QStringListModel>

class AddPageCommand : public QUndoCommand
{
public:
  AddPageCommand(int currentPage, QList<Canvas*>& list,Canvas::Tool tool, QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

  Canvas *canvas() const;

  static QStringListModel* getPagesModel();
  static void setPagesModel(QStringListModel* pagesModel);

private:
  int m_currentPage;
  Canvas* m_canvas;
  QList<Canvas*>& m_list;
  Canvas::Tool m_currentTool;
  static QStringListModel* m_pagesModel;
};

#endif // ADDPAGECOMMAND_H
