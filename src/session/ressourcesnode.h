/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef RESSOURCESNODE_H
#define RESSOURCESNODE_H
#include <QString>

/**
  * @brief RessourceNode is part of the composite design pattern, it's the abstract class
  * @brief providing the basic API for ressources, Shortname and has children.
  */
class RessourcesNode
{
public:
    /**
      * @brief pure virtual method to get the shortname
      * @return reference to the short name
      */
    virtual const QString& getShortName() const=0;
    /**
      * @brief pure virtual shortname setter
      * @param new value
      */
    virtual void setShortName(QString& name)=0;
    /**
      * @brief allows to know if this node has children
      * @return either has children or not
      */
    virtual bool hasChildren() const =0;
//    virtual void addChild(RessourcesNode*) =0;
};

#endif // RESSOURCESNODE_H
