#ifndef CONNECTIONPROFILE_H
#define CONNECTIONPROFILE_H

#include <QObject>
#include <QProperty>
#include <QString>
#include <optional>

#include "network/network_type.h"
#include "network_global.h"

typedef std::function<void(void)> StdFunc;
/**
 * @brief The ConnectionProfile class stores any data about the connection: Mode (client or server) or role (GM or
 * Player)
 */
class NETWORK_EXPORT ConnectionProfile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isServer READ isServer WRITE setServerMode NOTIFY serverChanged)
    Q_PROPERTY(QString address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(bool isGM READ isGM WRITE setGm NOTIFY gmChanged)
    Q_PROPERTY(QString title READ profileTitle WRITE setProfileTitle NOTIFY titleChanged)
    Q_PROPERTY(QString campaignPath READ campaignPath WRITE setCampaignPath NOTIFY campaignPathChanged)
    Q_PROPERTY(QColor playerColor READ playerColor WRITE setPlayerColor NOTIFY playerColorChanged)
    Q_PROPERTY(QString playerName READ playerName WRITE setPlayerName NOTIFY playerNameChanged)
    Q_PROPERTY(QString playerId READ playerId WRITE setPlayerId NOTIFY playerIdChanged)
    Q_PROPERTY(QByteArray playerAvatar READ playerAvatar WRITE setPlayerAvatar NOTIFY playerAvatarChanged)

    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    Q_PROPERTY(bool connectionInfoValid READ connectionInfoValid NOTIFY connectionInfoValidChanged)
    Q_PROPERTY(bool playerInfoValid READ playerInfoValid WRITE setPlayerInfoValid NOTIFY playerInfoValidChanged)
    Q_PROPERTY(bool campaignInfoValid READ campaignInfoValid WRITE setCampaignInfoValid NOTIFY campaignInfoValidChanged)
    Q_PROPERTY(bool charactersValid READ charactersValid WRITE setCharactersValid NOTIFY charactersValidChanged)
public:
    ConnectionProfile();

    virtual ~ConnectionProfile();
    QString profileTitle() const;
    QString playerName() const;
    QColor playerColor() const;
    QByteArray playerAvatar() const;
    QString playerId() const;
    QString address() const;
    quint16 port() const;
    bool isServer() const;
    bool isGM() const;
    QString campaignPath() const;
    QByteArray password() const;

    bool valid() const;
    bool connectionInfoValid() const;
    bool playerInfoValid() const;
    bool charactersValid() const;
    bool campaignInfoValid() const;

    const std::vector<connection::CharacterData>& characters();
    void setCharacters(const std::vector<connection::CharacterData>& characters);
    connection::CharacterData& character(int i);
    void addCharacter(const connection::CharacterData& data);
    void removeCharacter(int index);
    int characterCount();
    void clearCharacter();
    void cloneProfile(const ConnectionProfile* src);

public slots:
    void setProfileTitle(const QString&);
    void setPlayerColor(const QColor&);
    void setPlayerAvatar(const QByteArray&);
    void setPlayerName(const QString&);
    void setAddress(const QString&);
    void setPlayerId(const QString& playerId);
    void setPort(quint16);
    void setServerMode(bool);
    void setGm(bool);
    void setCampaignPath(const QString& id);
    void editPassword(const QByteArray& password);
    void setHash(const QByteArray& password);
    void setCharactersValid(bool val);
    void setCampaignInfoValid(bool val);
    void setPlayerInfoValid(bool b);

    void characterHasChanged();

signals:
    void serverChanged();
    void addressChanged();
    void portChanged();
    void gmChanged();
    void campaignPathChanged();
    void titleChanged();
    void playerColorChanged();
    void playerNameChanged();
    void playerIdChanged();
    void playerAvatarChanged();
    void characterCountChanged();
    void passwordChanged();
    void validChanged();
    void characterChanged();

    void playerInfoValidChanged();
    void charactersValidChanged();
    void campaignInfoValidChanged();
    void connectionInfoValidChanged();

private slots:
    void setValid(bool b);
    void setConnectionInfoValid(bool b);

private:
    // Profile data
    QString m_title; ///< @brief defines the name of the profile. It can be what ever users want.

    // Player data
    QString m_playerName;
    QColor m_playerColor;
    QByteArray m_playerAvatar;
    QString m_playerId;
    bool m_isGM= true;

    // Connection data
    bool m_server= false;
    quint16 m_port= 6660;
    QString m_address;
    QByteArray m_password;

    // campaign dir
    QString m_campaignDir;

    // Character info
    std::vector<connection::CharacterData> m_characters;

    bool m_valid{};
    bool m_validConnectionInfo{};
    bool m_validPlayerInfo{};
    bool m_validCharacter{};
    bool m_validCampaign{};
};

#endif // CONNECTIONPROFILE_H
