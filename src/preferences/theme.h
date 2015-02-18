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
private:
    QString m_name;
    QColor m_backgroundColor;
    QString m_backgroundImage;
    int m_position;
    bool m_repeat;


    QFont m_ChatFont;
    QColor m_chatBackgroundColor;
    QString m_chatBackgroundImage;
};

#endif // THEME_H
