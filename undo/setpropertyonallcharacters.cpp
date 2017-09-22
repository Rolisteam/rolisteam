#include "setfieldpropertycommand.h"

SetPropertyOnCharactersCommand::AddPageCommand(QUndoCommand *parent)
  : QUndoCommand(parent)
{
  setText(QObject::tr("Delete Character #%1").arg(index));
}

void SetPropertyOnCharactersCommand::undo()
{

}

void SetPropertyOnCharactersCommand::redo()
{


}
