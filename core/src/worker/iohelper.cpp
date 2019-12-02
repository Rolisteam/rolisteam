/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "iohelper.h"

#include <QColor>
#include <QDataStream>
#include <QFile>
#include <QString>
#include <QVariant>
#include <map>

#include "controller/view_controller/vectorialmapcontroller.h"
#include "data/cleveruri.h"
#include "vmap/vmap.h"

#include "vmap/items/characteritem.h"
#include "vmap/items/ellipsitem.h"
#include "vmap/items/imageitem.h"
#include "vmap/items/lineitem.h"
#include "vmap/items/pathitem.h"
#include "vmap/items/rectitem.h"
#include "vmap/items/textitem.h"

IOHelper::IOHelper() {}

bool IOHelper::loadVMap(VMap* vmap, CleverURI* uri, VectorialMapController* ctrl)
{
    if(vmap == nullptr || nullptr == uri || nullptr == ctrl)
        return false;

    auto path= uri->getUri();
    QIODevice* device= nullptr;
    if(!path.isEmpty() && uri->loadingMode() == CleverURI::Linked)
        device= new QFile(path);
    else if(uri->hasData() && uri->loadingMode() == CleverURI::Internal)
    {
        auto data= uri->getData();
        device= new QBuffer(&data);
    }

    if(!device->open(QIODevice::ReadOnly))
        return false;
    QDataStream reader(device);
    reader.setVersion(QDataStream::Qt_5_7);

    int zIndex, propertyCount, itemCount;
    QString name;
    QColor bgColor;

    reader >> name;
    reader >> bgColor;
    reader >> zIndex;
    reader >> propertyCount;

    ctrl->setName(name);
    ctrl->setBackgroundColor(bgColor);

    std::map<Core::Properties, QVariant> properties;
    for(int i= 0; i < propertyCount; ++i)
    {
        int pro;
        QVariant value;
        reader >> pro;
        reader >> value;
        properties.insert({static_cast<Core::Properties>(pro), value});
    }
    std::for_each(properties.begin(), properties.end(), [ctrl](const std::pair<Core::Properties, QVariant>& pair) {
        ctrl->setOption(pair.first, pair.second);
    });

    /*auto fogItem= vmap->getFogItem();
    reader >> *fogItem;*/

    reader >> itemCount;
    for(int i= 0; i < itemCount; ++i)
    {
        VisualItem* item= nullptr;
        int contentType;
        reader >> contentType;

        switch(static_cast<VisualItem::ItemType>(contentType))
        {
        case VisualItem::TEXT:
            item= new TextItem(properties);
            break;
        case VisualItem::CHARACTER:
            item= new CharacterItem(properties);
            break;
        case VisualItem::LINE:
            item= new LineItem(properties);

            break;
        case VisualItem::RECT:
            item= new RectItem(properties);
            break;
        case VisualItem::ELLISPE:
            item= new EllipsItem(properties);

            break;
        case VisualItem::PATH:
            item= new PathItem(properties);
            break;
        case VisualItem::SIGHT:
            item= vmap->getFogItem();
            break;
        case VisualItem::GRID:
            item= vmap->getGridItem();
            break;
        case VisualItem::IMAGE:
            item= new ImageItem();
            break;
        default:
            break;
        }
        if(nullptr == item)
            continue;

        reader >> *item;

        qreal x, y;
        reader >> x;
        reader >> y;
        // addNewItem(new AddVmapItemCommand(item, false, this), false);
        vmap->addItem(item);

        item->setPos(x, y);
        item->initChildPointItem();
    }
    return true;
}
