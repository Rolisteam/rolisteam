#ifndef DICESHORTCUT_H
#define DICESHORTCUT_H

#include <QString>

class DiceShortCut
{
public:
    DiceShortCut();

    QString text() const;
    void setText(const QString &text);

    QString command() const;
    void setCommand(const QString &command);

    bool alias() const;
    void setAlias(bool alias);

private:
    QString m_text;
    QString m_command;
    bool m_alias;
};

#endif // DICESHORTCUT_H
