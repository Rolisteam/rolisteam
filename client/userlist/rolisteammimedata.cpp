#include "rolisteammimedata.h"


RolisteamMimeData::RolisteamMimeData()
    : m_data(nullptr)
{
    m_format = "rolisteam/userlist-item";
}
bool RolisteamMimeData::hasFormat ( const QString & mimeType ) const
{
    if(mimeType==m_format)
    {
        return hasPerson();
    }
    return QMimeData::hasFormat(mimeType);
}

void RolisteamMimeData::setPerson(Person* data)
{
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
