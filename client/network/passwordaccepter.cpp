#include "passwordaccepter.h"
#include <QCryptographicHash>

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

    if(nullptr != m_next)
    {
        result&=m_next->isValid(data);
    }

    return result;
}
