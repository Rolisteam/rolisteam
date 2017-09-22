#include "deletecharactercommand.h"

DeleteCharacterCommand::AddPageCommand(QUndoCommand *parent)
  : QUndoCommand(parent)
{
  setText(QObject::tr("Delete Character #%1").arg(index));
}

void DeleteCharacterCommand::undo()
{

}

void DeleteCharacterCommand::redo()
{


}
