#ifndef THEME_H
#define THEME_H

#include <QString>
#include <QFont>
#include <QColor>

class Theme
{
public:
    Theme();

    QString name() const;
    void setName(QString name);

    QString backgroundImage() const;
    void setBackgroundImage(QString bgimage);

    QColor backgroundColor()const;
    void setBackgroundColor(QColor color);

    QFont chatFont() const;
    void setChatFont(QFont font);

    QColor chatColor() const;
    void setChatColor(QColor color);

    QString chatBackGroundImage() const;
    void setChatBackGroundImage(QString uri);



    friend QDataStream& operator<<(QDataStream& os,const Theme&);
    friend QDataStream& operator>>(QDataStream& is,Theme&);

private:
    QString m_name;
    QColor m_backgroundColor;
    QString m_backgroundImage;
    int m_position;
    bool m_repeat;


    QFont m_chatFont;
    QColor m_chatBackgroundColor;
    QString m_chatBackgroundImage;



};
typedef QList<Theme*> ThemeList;
#include <QVariant>
Q_DECLARE_METATYPE ( Theme )
Q_DECLARE_METATYPE ( Theme* )
Q_DECLARE_METATYPE ( ThemeList )
#endif // THEME_H
