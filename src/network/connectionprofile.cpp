#include "connectionprofile.h"

#define DEFAULT_PORT 6660

ConnectionProfile::ConnectionProfile()
    : m_character(NULL),m_server(false),m_port(DEFAULT_PORT),m_player(NULL),m_isGM(false)
{

}
ConnectionProfile::~ConnectionProfile()
{

}
void ConnectionProfile::setTitle(QString str)
{
    m_title = str;
}
void ConnectionProfile::setName(QString str)
{
    m_name = str;
}
void ConnectionProfile::setAddress(QString str)
{
    m_address = str;
}
void ConnectionProfile::setPort(int i)
{
    m_port = i;
}
void ConnectionProfile::setServerMode(bool b)
{
    m_server = b;
}
void ConnectionProfile::setPlayer(Player* p)
{
    m_player = p;
}
void ConnectionProfile::setGm(bool b)
{
    m_isGM = b;
}
QString ConnectionProfile::getTitle()const
{
    return m_title;
}
QString ConnectionProfile::getName() const
{
    return m_name;
}
QString ConnectionProfile::getAddress() const
{
    return m_address;
}
int ConnectionProfile::getPort() const
{
    return m_port;
}
bool ConnectionProfile::isServer() const
{
    return m_server;
}
Player* ConnectionProfile::getPlayer() const
{
    return m_player;
}
bool    ConnectionProfile::isGM() const
{
    return m_isGM;
}
Character* ConnectionProfile::getCharacter() const
{
    return m_character;
}
void ConnectionProfile::setCharacter(Character* character)
{
    m_character = character;
}
