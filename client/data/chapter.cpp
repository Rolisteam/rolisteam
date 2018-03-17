/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://www.rolisteam.org/contact                   *
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

#include <QDebug>
#include <QDataStream>
#include <QStyle>
#include <QApplication>

#include "chapter.h"
#include "character.h"

Chapter::Chapter()
    : m_children(QList<ResourcesNode*>())
{
}
Chapter::Chapter(const Chapter& m)
:QObject(m.parent()),m_children(QList<ResourcesNode*>())
{
    m_name=m.m_name;
}

Chapter::~Chapter()
{
    qDeleteAll(m_children);
    m_children.clear();
}

bool Chapter::hasChildren() const
{
    return !m_children.isEmpty();
}

bool Chapter::mayHaveChildren() const
{
    return true;
}

int Chapter::getChildrenCount() const
{
 return m_children.count();
}


ResourcesNode *Chapter::getChildAt(int i) const
{
    if((i>=0)&&(i<m_children.size()))
    {
        return m_children[i];
    }
    return nullptr;
}

int Chapter::indexOf(ResourcesNode* node)
{
    return m_children.indexOf(node);
}

void Chapter::addResource(ResourcesNode* cluri)
{
    m_children.append(cluri);
    cluri->setParentNode(this);
}
QVariant Chapter::getData(ResourcesNode::DataValue i)
{
    if(ResourcesNode::NAME==i)
    {
        return m_name;
    }
    return QVariant();
}
void Chapter::insertChildAt(int row, ResourcesNode* uri)
{
    m_children.insert(row,uri);
    uri->setParentNode(this);
}

bool Chapter::seekNode(QList<ResourcesNode *> &path, ResourcesNode *node)
{
    bool val = false;
    if(m_children.contains(node))
    {
        path.append(this);
        path.append(node);
        val = true;
    }
    else
    {
        for(auto child : m_children)
        {
            if((child->hasChildren()) && (child->seekNode(path,node)))
            {
                val = true;
                path.prepend(this);
            }
        }
    }
    return val;
}

QIcon Chapter::getIcon()
{
    QStyle* style = qApp->style();

    return style->standardIcon(QStyle::SP_DirIcon);
}

ResourcesNode::TypeResource Chapter::getResourcesType() const
{
    return ResourcesNode::Chapter;
}

bool Chapter::contains(ResourcesNode* node)
{
    if(m_children.contains(node))
    {
        return true;
    }
    else
    {
        for(ResourcesNode* child: m_children)
        {
            if(child->contains(node))
            {
                return true;
            }
        }
    }
    return false;
}

void Chapter::write(QDataStream &out, bool , bool ) const
{
    out << QStringLiteral("Chapter");
    out << m_name;
    out << m_children.size();
    for(const ResourcesNode* node : m_children)
    {
        node->write(out);
    }
}

void Chapter::read(QDataStream &in)
{
    in >> m_name;
    int count;
    in >> count;
    for(int i=0;i< count;++i)
    {
        QString type;
        in >> type;
        ResourcesNode* node;
        CleverURI* uri=nullptr;
        if(type=="Chapter")
        {
            Chapter* chapter = new Chapter();
            node=chapter;
            connect(chapter,&Chapter::openFile,this,&Chapter::openFile);
            connect(chapter,&Chapter::openResource,this,&Chapter::openResource);
        }
        else if(type=="Character")
        {
            Character* character = new Character();
            node = character;
        }
        else
        {
            uri = new CleverURI();
            node = uri;
        }
        node->setParentNode(this);
        m_children.append(node);
        node->read(in);
        if(nullptr!=uri)
        {
            if(uri->isDisplayed())
            {
                //emit openFile(uri,true);
                emit openResource(uri, true);
            }
        }
    }
}

bool Chapter::removeChild(ResourcesNode* item)
{ 
    bool removed=false;
    if(m_children.contains(item))
    {
        removed = m_children.removeOne(item);
    }
    else
    {
        for(ResourcesNode* child : m_children)
        {
            if(child->mayHaveChildren())
            {
                Chapter* chap = dynamic_cast<Chapter*>(child);
                if(nullptr!=chap)
                {
                    removed = chap->removeChild(item);
                }
            }
        }
    }
    return removed;
}
QDataStream& operator<<(QDataStream& os,const Chapter& chap)
{
    chap.write(os);
    return os;
}

QDataStream& operator>>(QDataStream& is,Chapter& chap)
{
    chap.read(is);
    return is;
    
}
