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

#include "resourcesnode.h"

class Character;
class NetworkMessageWriter;
class Player;

/**
 * @brief Abstract class for players and characters.
 */
class Person : public ResourcesNode
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QImage avatar READ getAvatar WRITE setAvatar NOTIFY avatarChanged)
    Q_PROPERTY(QString avatarPath READ avatarPath WRITE setAvatarPath NOTIFY avatarPathChanged)
public:
    Person();
    Person(const QString& name, const QColor& color, const QString& uuid= QString());
    /**
     * @brief ~Person
     */
    virtual ~Person() override;
    /**
     * @brief color
     * @return
     */
    QColor getColor() const;
    /**
     * @brief parent
     * @return
     */
    virtual Person* parentPerson() const;
    void setParentPerson(Person* parent);
    /**
     * @brief gives access to person's avatar.
     */
    virtual const QImage& getAvatar() const;

    /**
     * @brief gives access to person's avatar.
     */
    virtual bool hasAvatar() const;
    /**
     * @brief checkedState
     * @return
     */
    Qt::CheckState checkedState();
    /**
     * @brief isLeaf
     * @return
     */
    virtual bool isLeaf() const override;
    /**
     * @brief setState
     * @param c
     */
    void setState(Qt::CheckState c);

    virtual QHash<QString, QString> getVariableDictionnary()= 0;

    virtual QVariant getData(ResourcesNode::DataValue) const override;

    virtual QIcon icon() const override;

    ResourcesNode::TypeResource type() const override;
    virtual void write(QDataStream& out, bool tag= true, bool saveData= true) const override;
    virtual void read(QDataStream& in) override;
    virtual QString avatarPath() const;

public slots:
    /**
     * @brief setColor
     * @param color
     * @return
     */
    bool setColor(const QColor& color);
    /**
     * @brief set the person's avatar.
     */
    virtual void setAvatar(const QImage& p);
    void setAvatarPath(const QString& avatarPath);

signals:
    void colorChanged();
    void avatarChanged();
    void avatarPathChanged();

protected:
    QColor m_color;
    QImage m_avatar;
    QString m_avatarPath;
    Person* m_parentPerson= nullptr;
    Qt::CheckState m_checkState;
};

#endif
