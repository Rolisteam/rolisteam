#include "connectionprofile.h"

#include <QCryptographicHash>
#include <QDebug>

ConnectionProfile::ConnectionProfile()
    : m_title(QObject::tr("Unknown")), m_playerName(QObject::tr("Player")), m_playerColor(Qt::red)
{
    {
        CharacterData data({QObject::tr("Unknown Character"), Qt::red, "", QHash<QString, QVariant>()});
        addCharacter(data);
    }
    auto charact= character(0);
    qDebug() << charact.m_name;
}
ConnectionProfile::~ConnectionProfile() {}
void ConnectionProfile::setProfileTitle(const QString& str)
{
    m_title= str;
}
void ConnectionProfile::setPlayerName(const QString& str)
{
    m_playerName= str;
}
void ConnectionProfile::setAddress(const QString& str)
{
    m_address= str;
}
void ConnectionProfile::setPort(quint16 i)
{
    m_port= i;
}
void ConnectionProfile::setServerMode(bool b)
{
    m_server= b;
}

void ConnectionProfile::setGm(bool b)
{
    m_isGM= b;
}
QString ConnectionProfile::profileTitle() const
{
    return m_title;
}
QString ConnectionProfile::playerName() const
{
    return m_playerName;
}

void ConnectionProfile::setPlayerColor(const QColor& color)
{
    m_playerColor= color;
}

QColor ConnectionProfile::playerColor() const
{
    return m_playerColor;
}

void ConnectionProfile::setPlayerAvatar(const QString& path)
{
    m_playerAvatar= path;
}

QString ConnectionProfile::playerAvatar() const
{
    return m_playerAvatar;
}
QString ConnectionProfile::address() const
{
    return m_address;
}
quint16 ConnectionProfile::port() const
{
    return m_port;
}
bool ConnectionProfile::isServer() const
{
    return m_server;
}

bool ConnectionProfile::isGM() const
{
    return m_isGM;
}

QByteArray ConnectionProfile::password() const
{
    return m_password;
}

void ConnectionProfile::setPassword(const QString& password)
{
    if(password.isEmpty())
    {
        m_password.clear();
    }
    else
    {
        m_password= QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512);
    }
}
void ConnectionProfile::setHash(const QByteArray& password)
{
    m_password= password;
}

const std::vector<CharacterData>& ConnectionProfile::characters()
{
    return m_characters;
}
void ConnectionProfile::addCharacter(const CharacterData& data)
{
    m_characters.push_back(data);
}

CharacterData& ConnectionProfile::character(int i)
{
    qDebug() << "ConnectionProfile::character" << m_characters.size();
    return m_characters[static_cast<std::size_t>(i)];
}

int ConnectionProfile::characterCount()
{
    return static_cast<int>(m_characters.size());
}

void ConnectionProfile::removeCharacter(int index)
{
    m_characters.erase(m_characters.begin() + index);
}

void ConnectionProfile::clearCharacter()
{
    m_characters.clear();
}

void ConnectionProfile::cloneProfile(const ConnectionProfile* src)
{
    m_password= src->password();
    setGm(src->isGM());
    setPort(src->port());
    setProfileTitle(src->profileTitle());
    setPlayerName(src->playerName());
    setAddress(src->address());
    setServerMode(src->isServer());
}
