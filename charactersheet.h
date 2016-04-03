/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
    *                                                                         *
    *   This program is free software; you can redistribute it and/or modify  *
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

#ifndef CHARACTERSHEET_H
#define CHARACTERSHEET_H
#include <QString>
#include <QMap>
#include <QVariant>

#ifndef RCSE
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#endif
#include "charactersheetitem.h"

/**
    * @brief the characterSheet stores Section as many as necessary
    */
class CharacterSheet : public QObject
{
    Q_OBJECT

    //Q_PROPERTY(QString values NOTIFY valuesChanged)
public:
    
    /**
    * Constructor
    */
    CharacterSheet();
    /**
    * @brief allows to get the key, this function is used for displaying the meaning of fields
    * @param int index : 0 refers to the title of the section, 1 refers to key of the first data of the first section...
    */
    const QString getkey(int index);
    /**
    * @brief return the number of fields: Sum(number of section + sum of all sections items.)
    */
    int getIndexCount();
    
    virtual void save(QJsonObject& json);
    virtual void load(QJsonObject& json);

    #ifndef RCSE
    void fill(NetworkMessageWriter & message);
    void read(NetworkMessageReader &msg);
    #endif

    const QString getTitle();
signals:
    void valuesChanged(QString valueKey,QString value);

public slots:
    /**
    * @brief global getter of data.  This function has been written to make easier the MVC architecture.
    * @param QString path : 0 refers to the title of the first section, 1 refers to the first data of the first section....
    */
    const  QString getValue(QString key) const;
    void setValue(QString key , QString value);

private:
    QStringList explosePath(QString);


private:
    QMap<QString,QString> m_values;
    /**
    *@brief User Id of the owner
    */
    QString m_name;
    static int m_count;
};

#endif // CHARACTERSHEET_H
