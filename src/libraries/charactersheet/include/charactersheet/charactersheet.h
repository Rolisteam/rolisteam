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

#include <charactersheet/charactersheet_global.h>
#include <charactersheet/charactersheetitem.h>

class Section;
class CSItem;
/**
 * @brief the characterSheet stores Section as many as necessary
 */
class CHARACTERSHEET_EXPORT CharacterSheet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
public:
    /**
     * Constructor
     */
    CharacterSheet(QObject* parent= nullptr);
    virtual ~CharacterSheet();
    const QString getkey(int index);
    virtual void save(QJsonObject& json) const;
    virtual void load(const QJsonObject& json);
    const QString getTitle();
    int getFieldCount() const;
    int indexFromId(const QString& id) const;
    CSItem* getFieldAt(int i) const;

    TreeSheetItem* getFieldFromKey(QString key) const;
    QHash<QString, QString> getVariableDictionnary();

    void insertCharacterItem(CSItem* item);

    QString uuid() const;
    void setUuid(const QString& uuid);
    QString name() const;
    void setName(const QString& name);

    virtual void setFieldData(const QJsonObject& obj, const QString& parent, bool network= false);
    void setOrigin(Section*);

    QList<QString> getAllDependancy(QString key);
    CSItem* getFieldFromIndex(const std::vector<int>& row) const;

    const QVariant getValueByIndex(const std::vector<int>& row, QString key,
                                   Qt::ItemDataRole role= Qt::DisplayRole) const;
    const QVariant getValue(QString path, int role= Qt::DisplayRole) const;

    bool removeField(const QString& path);
public slots:
    CSItem* setValue(QString key, QString value, QString formula);

signals:
    void updateField(CharacterSheet*, CSItem*, const QString& path);
    void updateTableFieldCellValue(CharacterSheet*, const QString& path, int r, int c);
    void fieldValueChanged(CharacterSheet*, const QString& id);
    void addLineToTableField(CharacterSheet*, CSItem*);
    void uuidChanged();
    void nameChanged();

protected:
    void insertField(QString key, CSItem* itemSheet);

private:
    QStringList explosePath(QString);

private:
    QMap<QString, CSItem*> m_valuesMap;
    QString m_name;
    static int m_count;
    QString m_uuid;
};

#endif // CHARACTERSHEET_H
