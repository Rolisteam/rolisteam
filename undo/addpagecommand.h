#ifndef ADDPAGECOMMAND_H
#define ADDPAGECOMMAND_H

#include <QUndoCommand>

class AddPageCommand : public QUndoCommand
{
public:
  AddPageCommand(QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:

};

#endif // ADDPAGECOMMAND_H
