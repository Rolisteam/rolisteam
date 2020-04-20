/***************************************************************************
 *	 Copyright (C) 2009 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#include <QMap>
#include <QString>
#include <QVariant>

#include "field.h"

class Section;
#include "charactersheetitem.h"

/**
 * @brief the characterSheet stores Section as many as necessary
 */
class CharacterSheet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
public:
    /**
     * Constructor
     */
    CharacterSheet();
    virtual ~CharacterSheet();
    /**
     * @brief allows to get the key, this function is used for displaying the meaning of fields
     * @param int index : 0 refers to the title of the section, 1 refers to key of the first data of the first
     * section...
     */
    const QString getkey(int index);
    /**
     * @brief save
     * @param json
     */
    virtual void save(QJsonObject& json) const;
    /**
     * @brief load
     * @param json
     */
    virtual void load(const QJsonObject& json);

    /**
     * @brief getTitle
     * @return
     */
    const QString getTitle();
    /**
     * @brief getFieldCount
     * @return
     */
    int getFieldCount();
    /**
     * @brief getFieldAt
     * @param i
     * @return
     */
    CharacterSheetItem* getFieldAt(int i) const;

    /**
     * @brief getFieldFromKey
     * @param key
     * @return
     */
    CharacterSheetItem* getFieldFromKey(QString key) const;

    /**
     * @brief getRootSection
     * @return
     */
    Section* getRootSection() const;
    /**
     * @brief setRootSection
     * @param rootSection
     */
    void buildDataFromSection(Section* rootSection);
    /**
     * @brief getVariableDictionnary
     * @return
     */
    QHash<QString, QString> getVariableDictionnary();

    void insertCharacterItem(CharacterSheetItem* item);

    QString uuid() const;
    void setUuid(const QString& uuid);
    QString name() const;
    void setName(const QString& name);

    void setFieldData(const QJsonObject& obj, const QString& parent);
    void setOrigin(Section*);

    QList<QString> getAllDependancy(QString key);
    CharacterSheetItem* getFieldFromIndex(const std::vector<int>& row) const;
    /**
     * @brief global getter of data.  This function has been written to make easier the MVC architecture.
     * @param QString path : 0 refers to the title of the first section, 1 refers to the first data of the first
     * section....
     */
    const QVariant getValueByIndex(const std::vector<int>& row, QString key,
                                   Qt::ItemDataRole role= Qt::DisplayRole) const;
    const QVariant getValue(QString path, int role= Qt::DisplayRole) const;

    bool removeField(const QString& path);
public slots:
    CharacterSheetItem* setValue(QString key, QString value, QString formula);

signals:
    void updateField(CharacterSheet*, CharacterSheetItem*, const QString& path);
    void addLineToTableField(CharacterSheet*, CharacterSheetItem*);
    void uuidChanged();
    void nameChanged();

protected:
    void insertField(QString key, CharacterSheetItem* itemSheet);

private:
    QStringList explosePath(QString);

private:
    QMap<QString, CharacterSheetItem*> m_valuesMap;
    /**
     *@brief User Id of the owner
     */
    QString m_name;
    static int m_count;
    Section* m_rootSection;
    QString m_uuid;
};

#endif // CHARACTERSHEET_H
