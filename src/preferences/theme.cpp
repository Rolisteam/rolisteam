#include "theme.h"

Theme::Theme()
{
}

QString Theme::name() const
{
    return m_name;
}
void Theme::setName(QString name)
{
    m_name=name;
}
