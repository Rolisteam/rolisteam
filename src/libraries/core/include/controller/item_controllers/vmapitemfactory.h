/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
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
#ifndef VMAPITEMFACTORY_H
#define VMAPITEMFACTORY_H

#include "controller/item_controllers/visualitemcontroller.h"

class VectorialMapController;
class NetworkMessageReader;
#include <core_global.h>
namespace vmap
{
class CORE_EXPORT VmapItemFactory
{
public:
    static vmap::VisualItemController* createVMapItem(VectorialMapController* mapCtrl, Core::SelectableTool tool,
                                                      const std::map<QString, QVariant>& param);
    static vmap::VisualItemController* createRemoteVMapItem(VectorialMapController* mapCtrl, NetworkMessageReader* msg);
};
} // namespace vmap
#endif // VMAPITEMFACTORY_H
