#include "connectionprofile.h"

#include <QCryptographicHash>
#include <QDebug>

ConnectionProfile::ConnectionProfile()
    : m_title(QObject::tr("Unknown")), m_playerName(QObject::tr("Player")), m_playerColor(Qt::red)
{
    connect(this, &ConnectionProfile::gmChanged, this, [this]() {
        if(!m_isGM && m_characters.empty())
        {
            connection::CharacterData data({QObject::tr("Unknown Character"), Qt::red, "", QHash<QString, QVariant>()});
            addCharacter(data);
        }
    });
}
ConnectionProfile::~ConnectionProfile() {}
void ConnectionProfile::setProfileTitle(const QString& str)
{
    if(str == m_title)
        return;
    m_title= str;
    emit titleChanged();
}
void ConnectionProfile::setPlayerName(const QString& str)
{
    if(str == m_playerName)
        return;
    m_playerName= str;
    emit playerNameChanged();
}
void ConnectionProfile::setAddress(const QString& str)
{
    if(str == m_address)
        return;
    m_address= str;
    emit addressChanged();
}
void ConnectionProfile::setPort(quint16 i)
{
    if(i == m_port)
        return;
    m_port= i;
    emit portChanged();
}
void ConnectionProfile::setServerMode(bool b)
{
    if(b == m_server)
        return;
    m_server= b;
    emit serverChanged();
}

void ConnectionProfile::setGm(bool b)
{
    if(m_isGM == b)
        return;
    m_isGM= b;
    emit gmChanged();
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
    if(color == m_playerColor)
        return;
    m_playerColor= color;
    emit playerColorChanged();
}

QColor ConnectionProfile::playerColor() const
{
    return m_playerColor;
}

void ConnectionProfile::setPlayerAvatar(const QByteArray& data)
{
    if(data == m_playerAvatar)
        return;
    m_playerAvatar= data;
    emit playerAvatarChanged();
}

QByteArray ConnectionProfile::playerAvatar() const
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

QString ConnectionProfile::campaignPath() const
{
    return m_campaignDir;
}

void ConnectionProfile::setCampaignPath(const QString& id)
{
    if(id == m_campaignDir)
        return;
    m_campaignDir= id;
    emit campaignPathChanged();
}

QByteArray ConnectionProfile::password() const
{
    return m_password;
}

QString ConnectionProfile::playerId() const
{
    return m_playerId;
}

void ConnectionProfile::setPlayerId(const QString& playerId)
{
    if(playerId == m_playerId)
        return;
    m_playerId= playerId;
    emit playerIdChanged();
}

void ConnectionProfile::editPassword(const QString& password)
{
    if(password.isEmpty())
    {
        setHash({});
    }
    else
    {
        setHash(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha3_512));
    }
}
void ConnectionProfile::setHash(const QByteArray& password)
{
    qDebug() << "password hassh changed: " << profileTitle() << this;
    if(password == m_password)
        return;
    m_password= password;
    emit passwordChanged();
}

const std::vector<connection::CharacterData>& ConnectionProfile::characters()
{
    return m_characters;
}
void ConnectionProfile::addCharacter(const connection::CharacterData& data)
{
    m_characters.push_back(data);
    emit characterCountChanged();
}

connection::CharacterData& ConnectionProfile::character(int i)
{
    return m_characters[static_cast<std::size_t>(i)];
}

int ConnectionProfile::characterCount()
{
    return static_cast<int>(m_characters.size());
}

void ConnectionProfile::removeCharacter(int index)
{
    m_characters.erase(m_characters.begin() + index);
    emit characterCountChanged();
}

void ConnectionProfile::clearCharacter()
{
    m_characters.clear();
    emit characterCountChanged();
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
