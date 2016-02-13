/***************************************************************************
    *	 Copyright (C) 2009 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

class CharacterSheetItem
{
public:
    CharacterSheetItem();

    QString name() const;
    void setName(const QString &name);


    virtual QString getValue(int idColumn) const;
    virtual void setValue(QString,int idColunm);

    virtual bool mayHaveChildren()const =0;

    virtual int getChildrenCount()const=0;
    virtual CharacterSheetItem* getChildAt(QString) const=0;
    virtual CharacterSheetItem* getChildAt(int) const;

    virtual QString getPath();

    CharacterSheetItem* getParent() const;
    void setParent(CharacterSheetItem *parent);

    virtual int indexOf(CharacterSheetItem*)=0;

    int rowInParent();

    virtual void save(QJsonObject&)=0;
    virtual void load(QJsonObject&)=0;

    bool isReadOnly() const;
    void setReadOnly(bool readOnly);

protected:
    QString m_name;
    QStringList m_value;
    bool m_readOnly;
    CharacterSheetItem* m_parent;
};

class Field : public CharacterSheetItem
{
public:
    Field();

    int getChildrenCount()const;
    virtual bool mayHaveChildren()const;
    virtual CharacterSheetItem* getChildAt(QString i) const;
    int indexOf(CharacterSheetItem*);

    virtual void save(QJsonObject&);
    virtual void load(QJsonObject&);
};

/**
    * @brief Section stores any fields of specific section
    */
class  Section : public CharacterSheetItem
{
    
public:
    /**
    * Constructor
    */
    Section();
    Section(const Section* copy);

    int getChildrenCount() const;
    virtual CharacterSheetItem* getChildAt(QString i) const;
    virtual CharacterSheetItem* getChildAt(int i) const;
    virtual bool mayHaveChildren() const;

    virtual bool appendChild(CharacterSheetItem*);

    int indexOf(CharacterSheetItem *child);

    virtual void save(QJsonObject& json);
    virtual void load(QJsonObject& json);
    QStringList keyList() const;
    void setKeyList(const QStringList &keyList);

private:
    /**
     * @brief m_list
     */
    QHash<QString,CharacterSheetItem*> m_dataHash;
    QStringList m_keyList;
};
/**
    * @brief the characterSheet stores Section as many as necessary
    */
class CharacterSheet : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString values NOTIFY valuesChanged)
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

    void fill(NetworkMessageWriter & message);

    const QString getTitle();
    void read(NetworkMessageReader &msg);
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
