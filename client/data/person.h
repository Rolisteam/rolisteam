/*************************************************************************
 *     Copyright (C) 2011 by Joseph Boudou                               *
 *      Copyright (C) 2014 by Renaud Guezennec                            *
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
class Person : public QObject, public ResourcesNode
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QImage avatar READ getAvatar WRITE setAvatar NOTIFY avatarChanged)
public:
    /**
     * @brief Person
     * @param name
     * @param color
     */
    Person(const QString& name, const QColor& getColor);
    /**
     * @brief Person
     * @param uuid
     * @param name
     * @param color
     */
    Person(const QString& uuid, const QString& getName, const QColor& getColor);
    /**
     * @brief ~Person
     */
    virtual ~Person() override;
    /**
     * @brief getUuid
     * @return
     */
    const QString getUuid() const;
    virtual void setName(const QString& name) override;
    /**
     * @brief setColor
     * @param color
     * @return
     */
    bool setColor(const QColor& color);
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
     * @brief fill
     * @param message
     */
    virtual void fill(NetworkMessageWriter& message, bool withAvatar= true)= 0;

    /**
     * @brief gives access to person's avatar.
     */
    virtual const QImage& getAvatar() const;

    /**
     * @brief gives access to person's avatar.
     */
    virtual bool hasAvatar() const;

    /**
     * @brief set the person's avatar.
     */
    virtual void setAvatar(const QImage& p);
    /**
     * @brief checkedState
     * @return
     */
    Qt::CheckState checkedState();
    /**
     * @brief isLeaf
     * @return
     */
    virtual bool isLeaf() const;
    /**
     * @brief setState
     * @param c
     */
    void setState(Qt::CheckState c);

    virtual QHash<QString, QString> getVariableDictionnary()= 0;

    virtual QVariant getData(ResourcesNode::DataValue) const override;

    virtual QString getToolTip() const;

    virtual QIcon getIcon() override;

    ResourcesNode::TypeResource getResourcesType() const override;
    virtual void write(QDataStream& out, bool tag= true, bool saveData= true) const override;
    virtual void read(QDataStream& in) override;
    virtual bool seekNode(QList<ResourcesNode*>& path, ResourcesNode* node) override;

signals:
    void nameChanged();
    void colorChanged();
    void avatarChanged();

protected:
    /**
     * @brief Person
     */
    Person();
    QString m_uuid;
    QColor m_color;
    QImage m_avatar;
    Person* m_parentPerson= nullptr;
    Qt::CheckState m_checkState;

private:
    //    friend class PlayersList;
};

#endif
