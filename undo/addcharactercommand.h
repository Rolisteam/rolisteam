#ifndef ADDCHARACTERCOMMAND_H
#define ADDCHARACTERCOMMAND_H

#include <QUndoCommand>

class AddCharacterCommand : public QUndoCommand
{
public:
  AddCharacterCommand(QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:

};

#endif // ADDCHARACTERCOMMAND_H
