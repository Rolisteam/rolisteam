#include "setbackgroundimagecommand.h"

SetBackgroundCommand::AddPageCommand(QUndoCommand *parent)
  : QUndoCommand(parent)
{
  setText(QObject::tr("Delete Character #%1").arg(index));
}

void SetBackgroundCommand::undo()
{

}

void SetBackgroundCommand::redo()
{


}
