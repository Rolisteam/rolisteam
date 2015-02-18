#ifndef THEME_H
#define THEME_H

#include <QString>
#include <QFont>
#include <QColor>
/**
  * @brief is dedicated to store all attributes which defines customisation of the Graphical User Interface.
  */
class Theme
{
public:
    /**
      * @brief default constructor
      */
    Theme();
    /**
      * @brief accessor to the name
      */
    QString name() const;
    /**
      * @brief sets the theme's name
      */
    void setName(QString name);
    /**
      * @brief accessor to the background Image
      */
    QString backgroundImage() const;
    /**
      * @brief sets the theme's Image
      */
    void setBackgroundImage(QString bgimage);
    /**
      * @brief accessor to the background color
      */
    QColor backgroundColor()const;
    /**
      * @brief sets the background color
      */
    void setBackgroundColor(QColor color);

    /**
      * @brief accessor to the chat font of this theme.
      */
    QFont chatFont() const;
    /**
      * @brief sets the chat font.
      */
    void setChatFont(QFont font);
    /**
      * @brief accessor to the color chat.
      * @todo may be it should be useful to remake the design of this class
      * @todo and used flyweight pattern (or something).
      */
    QColor chatColor() const;
    /**
      * @brief sets the tchat color.
      */
    void setChatColor(QColor color);
    /**
      * @brief accessor to the chat background image
      */
    QString chatBackGroundImage() const;
    /**
      * @brief sets the chat background Image.
      */
    void setChatBackGroundImage(QString uri);





private:
    QString m_name;
    QColor m_backgroundColor;
    QString m_backgroundImage;
    int m_position;
    bool m_repeat;


    QFont m_chatFont;
    QColor m_chatBackgroundColor;
    QString m_chatBackgroundImage;

    friend QDataStream& operator<<(QDataStream& os,const Theme&);
    friend QDataStream& operator>>(QDataStream& is,Theme&);

};
typedef QList<Theme> ThemeList;
#include <QVariant>
Q_DECLARE_METATYPE ( Theme )
Q_DECLARE_METATYPE ( Theme* )
Q_DECLARE_METATYPE ( ThemeList )
#endif // THEME_H
