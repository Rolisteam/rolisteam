#include "addpagecommand.h"

AddPageCommand::AddPageCommand(QUndoCommand *parent)
  : QUndoCommand(parent)
{
  setText(QObject::tr("Add Page #%1").arg(index));
}

void AddPageCommand::undo()
{

}

void AddPageCommand::redo()
{


}
