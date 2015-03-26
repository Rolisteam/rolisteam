/*************************************************************************
 *    Copyright (C) 2007 by Romain Campioni                              *
 *    Copyright (C) 2013 by Renaud Guezennec                             *
 *                                                                       *
 *    http://www.rolisteam.org/                                          *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#ifndef TYPES_H
#define TYPES_H

#include <QColor>
#include <QString>



enum ColorKind {ColorType, Erase, Veil, Unveil};


typedef struct
{
	ColorKind type;
    QColor color;
} SelectedColor;

// Entete des messages envoyes et recus via les sockets
typedef struct
{
    quint8 categorie;				// Contient une categorie d'action
    quint8 action;					// Contient l'action a effectuer
	quint32 dataSize;			// Indique la taille des donnees suivant l'entete
} MsgHeader;

#endif
