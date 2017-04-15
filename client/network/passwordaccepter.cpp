#include "passwordaccepter.h"
#include <QCryptographicHash>
#include <QDebug>
PasswordAccepter::PasswordAccepter()
{

}

bool PasswordAccepter::isValid(const QMap<QString, QVariant> &data)
{
    QString pw = data["password"].toString();
    QString upw = data["userpassword"].toString();

    bool result = true;
    if((!pw.isEmpty())&&(QCryptographicHash::hash(upw.toUtf8(),QCryptographicHash::Sha3_512) != pw.toUtf8()))
    {
        result = false;
    }
    qInfo() << result;
    if(nullptr != m_next)
    {
        result&=m_next->isValid(data);
    }
    qInfo() << result;
    return result;
}
