#ifndef DICESHORTCUT_H
#define DICESHORTCUT_H

#include <QMetaType>
#include <QString>
#include <core_global.h>
class CORE_EXPORT DiceShortCut
{
public:
    DiceShortCut();

    QString text() const;
    void setText(const QString& text);

    QString command() const;
    void setCommand(const QString& command);

    bool alias() const;
    void setAlias(bool alias);

private:
    QString m_text;
    QString m_command;
    bool m_alias;
};

Q_DECLARE_METATYPE(DiceShortCut)

#endif // DICESHORTCUT_H
