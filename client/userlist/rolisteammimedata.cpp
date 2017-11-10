#include "rolisteammimedata.h"


RolisteamMimeData::RolisteamMimeData()
    : m_data(nullptr)
{
    //m_format = "rolisteam/userlist-item";
}
bool RolisteamMimeData::hasFormat ( const QString & mimeType ) const
{
    if(mimeType==QStringLiteral("rolisteam/userlist-item"))
    {
        return hasPerson();
    }
    else if(mimeType==QStringLiteral("rolisteam/dice-command"))
    {
        return !m_alias.first.isEmpty();
    }
    return QMimeData::hasFormat(mimeType);
}

void RolisteamMimeData::setPerson(Person* data)
{
    m_format = "rolisteam/userlist-item";
    m_data=data;
}
bool RolisteamMimeData::hasPerson() const
{
    if(m_data!=nullptr)
        return true;
    else
        return false;
}
Person* RolisteamMimeData::getData() const
{
    return m_data;
}
std::pair<QString,QString> RolisteamMimeData::getAlias() const
{
    return m_alias;
}
void RolisteamMimeData::setAlias(QString key, QString command)
{
    m_format = "rolisteam/dice-command";
    m_alias = std::pair<QString,QString>(key,command);
}
