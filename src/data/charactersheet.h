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
//typedef QMap<QString,QString> Section;
/**
 * @brief Section stores any fields of specific section
 */
class Section : public QMap<QString,QString>
{
public:
	/**
	* Constructor
	*/
    Section();

	
	/**
	* @brief allows to get the section's name.
	* @return name QString
	*/
    QString getName();

	/**
	* @brief allows to set the name of this section.
	* @param QString name
	*/
    void setName(QString name);

private:
    QString m_name;


};
/**
 * @brief the characterSheet stores Section as many as necessary 
*/
class CharacterSheet
{
public:

	/**
	* Constructor
	*/
    CharacterSheet();

	/**
	* @brief allows to get the data stored in the given section and associated with the given key.
	* @param section Id of section
	* @param key the id key. 
	*/
    const QString getData(int section,const QString& key);
	/**
	* @brief allows to get the title(name) of the given section
	* @param section id
	*/
    const QString getTitleSection(int section);
	
	/**
	* @brief global getter of data.  This function has been writen to make easier the MVC architecture.
	* @param int index : 0 refers to the title of the first section, 1 refers to the first data of the first section....
	*/
    const  QString getData(int index);
	/**
	* @brief allows to get the key, this function is used for displaying the meaning of fields
	* @param int index : 0 refers to the title of the section, 1 refers to key of the first data of the first section...
	*/
    const QString getkey(int index);

	/**
	* @brief translate a index into a section index
	* @param int index : 0 refers to the first section, if the first section stored 4 fields, then 5 refers to the second section.
	*/
    int getSection(int index);

	/**
	* @brief allows to register data,
	* @param int index : as getData
	* @param value the value of the added data.
	* @param isHeader true when add a section. 
	*/
    void setData(int index,QVariant value,bool isHeader = false);
	/**
	* @brief return the number of fields: Sum(number of section + sum of all sections items.)
	*/
    int getIndexCount();

    void appendSection();

private:
	/**
	* @brief 
	*/
    QList<Section> m_sectionList;
    QString m_owner;
};

#endif // CHARACTERSHEET_H
