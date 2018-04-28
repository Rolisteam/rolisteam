#ifndef CONNECTIONPROFILE_H
#define CONNECTIONPROFILE_H

#include "data/player.h"
/**
 * @brief The ConnectionProfile class stores any data about the connection: Mode (client or server) or role (GM or Player)
 */
class ConnectionProfile
{
public:
    /**
     * @brief ConnectionProfile
     */
    ConnectionProfile();
    /**
     * @brief ~ConnectionProfile
     */
    virtual ~ConnectionProfile();
    /**
     * @brief setTitle
     */
    void setTitle(QString);
    /**
     * @brief setName
     */
    void setName(QString);
    /**
     * @brief setAddress
     */
    void setAddress(QString);
    /**
     * @brief setPort
     */
    void setPort(int);
    /**
     * @brief setServerMode
     */
    void setServerMode(bool);
    /**
     * @brief setPlayer
     */
    void setPlayer(Player*);
    /**
     * @brief setGm
     */
    void setGm(bool);
    /**
     * @brief getTitle
     * @return
     */
    QString getTitle()const;
    /**
     * @brief getName
     * @return
     */
    QString getName() const;
    /**
     * @brief getAddress
     * @return
     */
    QString getAddress() const;
    /**
     * @brief getPort
     * @return
     */
    int     getPort() const;
    /**
     * @brief isServer
     * @return
     */
    bool    isServer() const;
    /**
     * @brief getPlayer
     * @return
     */
    Player* getPlayer() const;
    /**
     * @brief isGM
     * @return
     */
    bool    isGM() const;
    /**
     * @brief getCharacter
     * @return
     */
    Character* getCharacter() const;
    /**
     * @brief setCharacter
     */
    void setCharacter(Character* );

    /**
     * @brief getPassword
     * @return
     */
    QByteArray getPassword() const;
    /**
     * @brief setPassword
     * @param password
     */
    void setPassword(const QString &password);
    void setHash(const QByteArray &password);

    /**
     * @brief cloneProfile
     * @param src
     */
    void cloneProfile(const ConnectionProfile* src);

private:
    Character* m_character;
    bool    m_server;
    int     m_port;
    Player*  m_player;
    bool    m_isGM;///<
    QString m_title;///< @brief defines the name of the profile. It can be what ever users want.
    QString m_name;
    QString m_address;
    QByteArray m_password;
};


#endif // CONNECTIONPROFILE_H
