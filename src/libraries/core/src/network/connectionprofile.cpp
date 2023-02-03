#include "network/connectionprofile.h"

#include <QCryptographicHash>
#include <QDebug>

ConnectionProfile::ConnectionProfile()
    : m_title(QObject::tr("Unknown")), m_playerName(QObject::tr("Player")), m_playerColor(Qt::red)
{
    connect(this, &ConnectionProfile::gmChanged, this,
            [this]()
            {
                if(!m_isGM && m_characters.empty())
                {
                    connection::CharacterData data(
                        {QObject::tr("Unknown Character"), Qt::red, "", QHash<QString, QVariant>()});
                    addCharacter(data);
                }
            });

    // binding

    auto updateValid= [this]() {
        setValid(m_validConnectionInfo && m_validPlayerInfo && m_validCharacter && m_validCampaign
                 && !m_title.isEmpty());
    };

    connect(this, &ConnectionProfile::playerInfoValidChanged, this, updateValid);
    connect(this, &ConnectionProfile::charactersValidChanged, this, updateValid);
    connect(this, &ConnectionProfile::campaignInfoValidChanged, this, updateValid);
    connect(this, &ConnectionProfile::connectionInfoValidChanged, this, updateValid);

    auto updateConnectionInfo
        = [this]() { setConnectionInfoValid(m_port > 0 && m_server ? m_server : !m_address.isEmpty()); };

    connect(this, &ConnectionProfile::serverChanged, this, updateConnectionInfo);
    connect(this, &ConnectionProfile::portChanged, this, updateConnectionInfo);
    connect(this, &ConnectionProfile::addressChanged, this, updateConnectionInfo);

    updateConnectionInfo();
    updateValid();

    // Signals
    /*m_handlers.push_back(std::make_optional(m_valid.onValueChanged(StdFunc([this]() { emit validChanged(); }))));
    m_handlers.push_back(std::make_optional(
        m_validConnectionInfo.onValueChanged(StdFunc([this]() { emit connectionInfoValidChanged(); }))));
    m_handlers.push_back(
        std::make_optional(m_validPlayerInfo.onValueChanged(StdFunc([this]() { emit playerInfoValidChanged(); }))));
    m_handlers.push_back(
        std::make_optional(m_validCharacter.onValueChanged(StdFunc([this]() { emit charactersValidChanged(); }))));
    m_handlers.push_back(
        std::make_optional(m_validCampaign.onValueChanged(StdFunc([this]() { emit campaignInfoValidChanged(); }))));*/
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
    if(m_port == i)
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
    if(m_campaignDir == id)
        return;
    m_campaignDir= id;
    emit campaignPathChanged();
}

QByteArray ConnectionProfile::password() const
{
    return m_password;
}

bool ConnectionProfile::valid() const
{
    return m_valid;
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
    setPlayerAvatar(src->playerAvatar());
    setPlayerColor(src->playerColor());
    setAddress(src->address());
    setCampaignPath(src->campaignPath());
    setServerMode(src->isServer());
}

void ConnectionProfile::setCharactersValid(bool val)
{
    if(m_validCharacter == val)
        return;
    m_validCharacter= val;
    emit charactersValidChanged();
}

void ConnectionProfile::setCampaignInfoValid(bool val)
{
    if(m_validCampaign == val)
        return;
    m_validCampaign= val;
    emit campaignInfoValidChanged();
}

void ConnectionProfile::setPlayerInfoValid(bool val)
{
    if(val == m_validPlayerInfo)
        return;
    m_validPlayerInfo= val;
    emit playerInfoValidChanged();
}

void ConnectionProfile::characterHasChanged()
{
    emit characterChanged();
}

void ConnectionProfile::setValid(bool b)
{
    if(m_valid == b)
        return;
    m_valid= b;
    emit validChanged();
}

void ConnectionProfile::setConnectionInfoValid(bool b)
{
    if(m_validConnectionInfo == b)
        return;
    m_validConnectionInfo= b;
    emit connectionInfoValidChanged();
}

bool ConnectionProfile::connectionInfoValid() const
{
    return m_validConnectionInfo;
}
bool ConnectionProfile::playerInfoValid() const
{
    return m_validPlayerInfo;
}
bool ConnectionProfile::charactersValid() const
{
    return m_validCharacter;
}
bool ConnectionProfile::campaignInfoValid() const
{
    return m_validCampaign;
}
