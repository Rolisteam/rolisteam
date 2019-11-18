#include "modelhelper.h"

#include "data/character.h"
#include "data/player.h"
#include "model/profilemodel.h"
#include "network/connectionprofile.h"

#include <QDebug>
#include <QFileInfo>
#include <QSettings>

namespace Settingshelper
{
void readConnectionProfileModel(ProfileModel* model)
{
    QSettings settings("rolisteam", "rolisteam");
    settings.beginGroup("ConnectionProfiles");
    int size= settings.beginReadArray("ConnectionProfilesArray");
    for(int i= 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        ConnectionProfile* profile= new ConnectionProfile();
        profile->setAddress(settings.value("address").toString());
        profile->setName(settings.value("name").toString());
        profile->setTitle(settings.value("title").toString());
        profile->setPort(static_cast<quint16>(settings.value("port").toInt()));
        profile->setServerMode(settings.value("server").toBool());
        profile->setGm(settings.value("gm").toBool());
        profile->setHash(QByteArray::fromBase64(settings.value("password").toByteArray()));

        QColor color= settings.value("PlayerColor").value<QColor>();
        auto playerAvatarPath= settings.value("playerAvatarPath").toString();
        Player* player= new Player(profile->getName(), color, profile->isGM());
        player->setAvatarPath(playerAvatarPath);
        if(QFile::exists(playerAvatarPath))
        {
            QImage img(playerAvatarPath);
            player->setAvatar(img);
        }
        // player->setUserVersion(m_version);
        profile->setPlayer(player);

        Character* character= new Character();
        character->setName(settings.value("CharacterName").toString());
        character->setAvatar(settings.value("CharacterPix").value<QImage>());
        auto path= settings.value("CharacterPath").toString();
        if(!QFileInfo::exists(path))
            qWarning() << ProfileModel::tr("Error: avatar for %2 path is invalid. No file at this path: %1")
                              .arg(path, character->name());
        character->setAvatarPath(settings.value("CharacterPath").toString());
        character->setColor(settings.value("CharacterColor").value<QColor>());
        character->setHealthPointsCurrent(settings.value("CharacterHp", character->getHealthPointsCurrent()).toInt());
        character->setHealthPointsMax(settings.value("CharacterMaxHp", character->getHealthPointsMax()).toInt());
        character->setHealthPointsMin(settings.value("CharacterMinHp", character->getHealthPointsMin()).toInt());
        character->setDistancePerTurn(
            settings.value("CharacterDistPerTurn", character->getDistancePerTurn()).toDouble());
        character->setState(character->getStateFromIndex(settings.value("CharacterState", 0).toInt()));
        character->setLifeColor(settings.value("CharacterLifeColor", character->getLifeColor()).value<QColor>());
        character->setInitCommand(settings.value("CharacterInitCmd", character->getInitCommand()).toString());
        character->setHasInitiative(settings.value("CharacterHasInit", false).toBool());

        character->setNpc(false);

        profile->setCharacter(character);
        model->appendProfile(profile);
    }
    settings.endArray();
    settings.endGroup();

    if(size == 0)
    {
        ConnectionProfile* profile= new ConnectionProfile();
        profile->setName(ProfileModel::tr("New Player"));
        profile->setTitle(ProfileModel::tr("Default"));
        profile->setPort(6660);
        profile->setServerMode(true);
        QColor color= Qt::black;
        Player* player= new Player(profile->getName(), color, profile->isGM());
        // player->setUserVersion(m_version);
        profile->setPlayer(player);

        Character* character= new Character(QStringLiteral("Unknown"), Qt::black, false);
        profile->setCharacter(character);
        model->appendProfile(profile);
    }
}

void writeConnectionProfileModel(ProfileModel* model)
{
    QSettings settings("rolisteam", "rolisteam");
    settings.beginGroup("ConnectionProfiles");

    auto size= model->rowCount(QModelIndex());
    settings.beginWriteArray("ConnectionProfilesArray", size);
    int i= 0;
    for(int i= 0; i < size; ++i)
    {
        auto profile= model->getProfile(i);

        if(nullptr == profile)
            continue;

        settings.setArrayIndex(i);
        Player* player= profile->getPlayer();
        Character* character= profile->getCharacter();

        settings.setValue("address", profile->getAddress());
        settings.setValue("name", profile->getName());
        settings.setValue("title", profile->getTitle());
        settings.setValue("server", profile->isServer());
        settings.setValue("port", profile->getPort());
        settings.setValue("gm", profile->isGM());
        settings.setValue("password", profile->getPassword().toBase64());
        settings.setValue("PlayerColor", player->getColor());
        settings.setValue("playerAvatarPath", player->avatarPath());

        if(nullptr == character)
            continue;
        settings.setValue("CharacterColor", character->getColor());
        QImage img= character->getAvatar();
        QVariant var;
        var.setValue(img);
        settings.setValue("CharacterPix", var);
        settings.setValue("CharacterName", character->name());
        settings.setValue("CharacterPath", character->avatarPath());
        settings.setValue("CharacterHp", character->getHealthPointsCurrent());
        settings.setValue("CharacterMaxHp", character->getHealthPointsMax());
        settings.setValue("CharacterMinHp", character->getHealthPointsMin());
        settings.setValue("CharacterDistPerTurn", character->getDistancePerTurn());
        settings.setValue("CharacterState", character->indexOfState(character->getState()));
        settings.setValue("CharacterLifeColor", character->getLifeColor());
        settings.setValue("CharacterInitCmd", character->getInitCommand());
        settings.setValue("CharacterHasInit", character->hasInitScore());

        ++i;
    }
    settings.endArray();
    settings.endGroup();
}
} // namespace Settingshelper
