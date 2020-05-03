/***************************************************************************
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   https://rolisteam.org/contact                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef RESSOURCESNODE_H
#define RESSOURCESNODE_H

#include <QIcon>
#include <QString>
#include <QVariant>
/**
 * @brief RessourceNode is part of the composite design pattern, it's the abstract class
 * @brief providing the basic API for ressources, Shortname and has children.
 */
class ResourcesNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(bool hasChildren READ hasChildren NOTIFY hasChildrenChanged)
    Q_PROPERTY(int childrenCount READ childrenCount NOTIFY childrenCountChanged)
    Q_PROPERTY(TypeResource resourcesType READ type CONSTANT)
    Q_PROPERTY(ResourcesNode* parentNode READ parentNode WRITE setParentNode NOTIFY parentNodeChanged)
public:
    enum DataValue
    {
        NAME,
        MODE,
        DISPLAYED,
        URI
    };
    Q_ENUM(DataValue)

    enum TypeResource
    {
        Cleveruri,
        Person,
        Chapter
    };
    Q_ENUM(TypeResource)

    ResourcesNode(TypeResource type);
    ResourcesNode(TypeResource type, const QString& uuid);
    virtual ~ResourcesNode();

    QString uuid() const;
    virtual void setUuid(const QString& uuid);

    QString name() const;
    virtual void setName(const QString& name);

    ResourcesNode* parentNode() const;

    virtual QString value() const;
    virtual void setValue(QString);

    virtual bool hasChildren() const;
    virtual bool isLeaf() const;
    virtual int childrenCount() const;
    virtual QIcon getIcon() const;
    virtual ResourcesNode::TypeResource type() const;
    virtual QVariant getData(ResourcesNode::DataValue) const= 0;

    virtual ResourcesNode* getChildAt(int) const;
    virtual bool contains(ResourcesNode*);
    virtual int indexOf(ResourcesNode*) const;
    virtual bool removeChild(ResourcesNode*);
    virtual void insertChildAt(int row, ResourcesNode*);
    int rowInParent();

    virtual void write(QDataStream& out, bool tag= true, bool saveData= true) const= 0;
    virtual void read(QDataStream& in)= 0;
    virtual bool seekNode(QList<ResourcesNode*>& path, ResourcesNode* node)= 0;

public slots:
    void setParentNode(ResourcesNode* parent);

signals:
    void uuidChanged(QString id);
    void nameChanged();
    void valueChanged();
    void hasChildrenChanged();
    void childrenCountChanged();
    void parentNodeChanged();

protected:
    QString m_uuid;
    QString m_name;
    QString m_value;
    ResourcesNode* m_parent= nullptr;
    TypeResource m_type;
};

#endif // RESSOURCESNODE_H
