#ifndef FIELDFACTORY_H
#define FIELDFACTORY_H

#include "charactersheet/charactersheetitem.h"

namespace FieldFactory
{
TreeSheetItem* createField(TreeSheetItem::TreeItemType type, bool addCount);
};

#endif // FIELDFACTORY_H
