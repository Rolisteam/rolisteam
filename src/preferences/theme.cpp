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
QString Theme::backgroundImage() const
{
    return m_backgroundImage;
}

void Theme::setBackgroundImage(QString bgimage)
{
    m_backgroundImage=bgimage;
}

QColor Theme::backgroundColor()const
{
    return m_backgroundColor;
}
void Theme::setBackgroundColor(QColor color)
{
    m_backgroundColor=color;
}

QFont Theme::chatFont()const
{
    return m_chatFont;
}
void Theme::setChatFont(QFont font)
{
m_chatFont=font;
}

QColor Theme::chatColor() const
{
    return m_chatBackgroundColor;
}
void Theme::setChatColor(QColor color)
{
    m_chatBackgroundColor=color;
}

QString Theme::chatBackGroundImage() const
{
    return m_chatBackgroundImage;
}
void Theme::setChatBackGroundImage(QString uri)
{
    m_chatBackgroundImage=uri;
}
QDataStream& operator<<(QDataStream& out, const Theme& con)
{
  out << con.backgroundColor();
  out << con.backgroundImage();
  out << con.chatBackGroundImage();
  out << con.chatColor();
  out << con.chatFont();
  return out;
}

QDataStream& operator>>(QDataStream& is,Theme& con)
{
  is >>(con.m_backgroundColor);
  is >>(con.m_backgroundImage);
  is >> (con.m_chatBackgroundImage);
  is >> (con.m_chatBackgroundColor);
  is >> (con.m_chatFont);
  return is;
}
