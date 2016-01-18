/***************************************************************************
    *	Copyright (C) 2009 by Renaud Guezennec                             *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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

#include "chapter.h"

Chapter::Chapter()
{
}
Chapter::Chapter(const Chapter& m)
{
    m_name=m.m_name;
}

Chapter::~Chapter()
{
    
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
}

int Chapter::indexOf(ResourcesNode* node)
{
    return m_children.indexOf(node);
}

void Chapter::addResource(ResourcesNode* cluri)
{
    m_children.append(cluri);
    cluri->setParent(this);
}


bool Chapter::contains(ResourcesNode* node)
{
    if(m_children.contains(node))
    {
        return true;
    }
    else
    {
        foreach(ResourcesNode* child, m_children)
        {
            if(child->contains(node))
            {
                return true;
            }
        }
    }
    return false;
}

void Chapter::write(QDataStream &out) const
{
    out << QStringLiteral("Chapter");
    out << m_name;
    out << m_children.size();
    foreach(ResourcesNode* node,m_children)
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
        if(type=="Chapter")
        {
            node = new Chapter();
        }
        else
        {
            node = new CleverURI();
        }
        node->read(in);
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
        foreach(ResourcesNode* child,m_children)
        {
            if(child->mayHaveChildren())
            {
                Chapter* chap = dynamic_cast<Chapter*>(child);
                if(NULL!=chap)
                {
                    removed = chap->removeRessourcesNode(item);
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
