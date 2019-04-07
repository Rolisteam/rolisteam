#include "passwordaccepter.h"
#include <QCryptographicHash>
#include <QDebug>
PasswordAccepter::PasswordAccepter(PasswordAccepter::Level level) : m_currentLevel(level) {}

bool PasswordAccepter::isValid(const QMap<QString, QVariant>& data) const
{
    QString pw;
    QString upw;
    upw= data["userpassword"].toString();

    if(Connection == m_currentLevel)
    {
        pw= data["ServerPassword"].toString();
    }
    else if(Admin == m_currentLevel)
    {
        pw= data["AdminPassword"].toString();
    }
    else if(Channel == m_currentLevel)
    {
        pw= data["ChannelPassword"].toString();
    }
    bool result= false;
    if(upw == pw)
    { //(QCryptographicHash::hash(upw.toUtf8(),QCryptographicHash::Sha3_512) != pw.toUtf8()))
        result= true;
    }
    // qInfo() << result;
    return result;
}
