#ifndef SETBACKGROUNDCOMMAND_H
#define SETBACKGROUNDCOMMAND_H

#include <QUndoCommand>

class SetBackgroundCommand : public QUndoCommand
{
public:
  SetBackgroundCommand(QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:

};

#endif // SETBACKGROUNDCOMMAND_H
