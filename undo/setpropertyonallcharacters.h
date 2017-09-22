#ifndef SETPROPERTYONALLCHARACTERCOMMAND_H
#define SETPROPERTYONALLCHARACTERCOMMAND_H

#include <QUndoCommand>

class SetPropertyOnCharactersCommand : public QUndoCommand
{
public:
  SetPropertyOnCharactersCommand(QUndoCommand *parent = 0);

  void undo() override;
  void redo() override;

private:

};

#endif // SETFIELDPROPERTYCOMMAND_H
