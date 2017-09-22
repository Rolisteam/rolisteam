#include "addcharactercommand.h"

AddCharacterCommand::AddCharacterCommand(QUndoCommand *parent)
  : QUndoCommand(parent)
{
  setText(QObject::tr("Add Character #%1").arg(index));
}

void AddCharacterCommand::undo()
{

}

void AddCharacterCommand::redo()
{


}
