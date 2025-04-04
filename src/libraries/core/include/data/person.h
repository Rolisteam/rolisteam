/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
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
#ifndef PERSONS_H
#define PERSONS_H

#include <QColor>
#include <QImage>
#include <QList>
#include <QMap>
#include <QObject>

#include "network_global.h"

class Character;
class Player;

/**
 * @brief Abstract class for players and characters.
 */
class NETWORK_EXPORT Person : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QByteArray avatar READ avatar WRITE setAvatar NOTIFY avatarChanged)
public:
    Person(QObject* parent= nullptr);
    Person(const QString& name, const QColor& color, const QString& uuid= QString());
    virtual ~Person() override;

    QString name() const;
    QString uuid() const;

    QColor getColor() const;
    virtual Person* parentPerson() const;
    void setParentPerson(Person* parent);
    virtual QByteArray avatar() const;
    virtual bool hasAvatar() const;
    virtual bool isLeaf() const;

    virtual QHash<QString, QString> getVariableDictionnary()= 0;
    virtual QIcon icon() const;

public slots:
    bool setColor(const QColor& color);
    virtual void setAvatar(const QByteArray& p);
    void setName(const QString& name);
    void setUuid(const QString& uuid);

signals:
    void uuidChanged(QString id);
    void nameChanged(QString id);
    void colorChanged();
    void avatarChanged();

protected:
    QString m_name;
    QString m_uuid;
    QColor m_color;
    QByteArray m_avatar;
    Person* m_parentPerson= nullptr;
};

#endif
