/*************************************************************************
 *     Copyright (C) 2015 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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
#ifndef PREFERENCESLISTENER_H
#define PREFERENCESLISTENER_H

#include <QString>
/**
 * @brief The PreferencesListener class is an abstract class which is part of listener pattern. Any class which to be
 * updated after modification of some preferences must be an PreferencesListener.
 */
class PreferencesListener
{
public:
    PreferencesListener();
    virtual void preferencesHasChanged(QString) = 0;


};

#endif // PREFERENCESLISTENER_H
