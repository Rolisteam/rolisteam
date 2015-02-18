#include "chapter.h"

Chapter::Chapter()
{
}

bool Chapter::hasChildren() const
{
    return (m_ressoucelist.size()>0)?true:false;
}
const QString& Chapter::getShortName() const
{
    return m_name;
}
 void Chapter::setName(QString& p )
 {
     m_name =p;
 }
