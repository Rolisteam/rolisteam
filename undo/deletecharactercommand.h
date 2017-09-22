#ifndef DELETECHARACTERCOMMAND_H
#define DELETECHARACTERCOMMAND_H

#include <QUndoCommand>

class DeleteCharacterCommand : public QUndoCommand
{
public:
  DeleteCharacterCommand(QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:

};

#endif // DELETECHARACTERCOMMAND_H
