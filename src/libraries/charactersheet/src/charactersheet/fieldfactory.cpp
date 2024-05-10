#include "charactersheet/fieldfactory.h"

#include "charactersheet/controllers/fieldcontroller.h"
#include "charactersheet/controllers/section.h"
#include "charactersheet/controllers/slidercontroller.h"
#include "charactersheet/controllers/tablefield.h"

namespace FieldFactory
{
TreeSheetItem* createField(TreeSheetItem::TreeItemType type, bool addCount)
{
    TreeSheetItem* res= nullptr;
    switch(type)
    {
    case TreeSheetItem::SectionItem:
        res= new Section();
        break;
    case TreeSheetItem::FieldItem:
        res= new FieldController(type, addCount);
        break;
    case TreeSheetItem::TableItem:
        res= new TableFieldController(addCount);
        break;
    case TreeSheetItem::SliderItem:
        res= new SliderController(addCount);
        break;
    }
    return res;
}
} // namespace FieldFactory
