/***************************************************************************
 *     Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify     *
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
#ifndef PERSON_H
#define PERSON_H
#include <QString>
#include <QColor>
#include <QImage>

/**
  * @brief is an abstract class which is part of composite pattern, it's representing any kind of person (real or character)
  */
class Person
{
public:
    /**
      * @brief constructor with argument
      * @param name of the person
      * @param Person's color
      * @param Person's avatar uri
      *
      */
    Person(QString name,QColor color,QString uri);
    /**
      * @brief default constructor
      */
    Person();

    /**
      * @brief pure virtual function, must be overriden in subclasses.
      */
    virtual bool hasChildren() const =0;
    /**
      * @brief accessor function to the person's name.
      */
    virtual const QString& getName() const;
    /**
      * @brief accessor function to the person's color.
      */
    virtual const QColor& getColor() const;
    /**
      * @brief set the person's name.
      */
    virtual void setName(QString);
    /**
      * @brief set the person's color.
      */
    virtual void setColor(QColor);
    /**
      * @brief gives access to person's avatar.
      */
    virtual const QImage& getAvatar() const;
    /**
      * @brief set the person's avatar.
      */
    virtual void setAvatar(QImage& p);
    /**
      * @brief set the person state.
      */
    virtual void setState(Qt::CheckState m);
    /**
      * @brief accessor function to the person's state.
      */
    virtual Qt::CheckState checkedState();

protected:
    QString m_name;
    QColor m_color;
    Qt::CheckState m_state;
    QImage m_avatar;
};

#endif // PERSON_H
