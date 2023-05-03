#include <QTest>

#include <QSignalSpy>

#include "controller/view_controller/selectconnprofilecontroller.h"
#include "model/profilemodel.h"
#include "network/characterdatamodel.h"
#include "network/connectionprofile.h"
#include "worker/iohelper.h"
#include <QAbstractItemModelTester>
#include <helper.h>
#include <limits>

struct CharacterInfo
{
    QString name;
    QColor color;
    QByteArray avatar;
};

class ProfileControllerTest : public QObject
{
    Q_OBJECT
public:
    ProfileControllerTest()= default;

private slots:
    void init();

    void clone();
    void clone_data();
    void serialization();
    void serialization_data();
    void canConnect();
    void canConnect_data();

    void connectTest();

private:
    std::unique_ptr<ProfileModel> m_profileModel;
    std::unique_ptr<SelectConnProfileController> m_ctrl;
    std::unique_ptr<QUndoStack> m_stack;
};
Q_DECLARE_METATYPE(CharacterInfo)
void ProfileControllerTest::init()
{
    m_profileModel.reset(new ProfileModel());
    m_ctrl.reset(new SelectConnProfileController(m_profileModel.get(), nullptr));
    new QAbstractItemModelTester(m_profileModel.get());
}

void ProfileControllerTest::clone()
{

    QFETCH(int, profileCount);
    QFETCH(int, characterCount);
    QFETCH(bool, avatarSquare);
    QFETCH(bool, isGM);
    QFETCH(bool, isServer);
    QFETCH(bool, expected);

    for(int i= 0; i < profileCount; ++i)
    {
        m_ctrl->addProfile();
        QCOMPARE(m_profileModel->rowCount(), i + 1);
        m_ctrl->setCurrentProfileIndex(i);
        auto profile= m_profileModel->getProfile(i);
        QSignalSpy nameSpy(profile, &ConnectionProfile::titleChanged);

        auto name= Helper::randomString();
        // set player info
        m_ctrl->setProfileName(name);
        nameSpy.wait(10);
        QCOMPARE(nameSpy.count(), 1);
        auto playername= Helper::randomString();
        QSignalSpy playerNameSpy(profile, &ConnectionProfile::playerNameChanged);
        m_ctrl->setPlayerName(playername);
        playerNameSpy.wait(10);
        QCOMPARE(playerNameSpy.count(), 1);

        m_ctrl->setPlayerAvatar(Helper::imageData(avatarSquare));

        // set Campaign info
        QTemporaryDir dir;
        dir.setAutoRemove(false);
        dir.isValid();
        auto path= dir.path();

        if(m_ctrl->isGameMaster() != isGM)
        {
            QSignalSpy gmSpy(profile, &ConnectionProfile::gmChanged);
            m_ctrl->setIsGameMaster(isGM);
            gmSpy.wait(10);
            QCOMPARE(gmSpy.count(), 1);
        }

        QSignalSpy campSpy(profile, &ConnectionProfile::campaignPathChanged);
        m_ctrl->setCampaignPath(path);
        campSpy.wait(10);

        QCOMPARE(campSpy.count(), 1);

        // set network info
        if(m_ctrl->isServer() != isServer)
        {
            QSignalSpy serverSpy(profile, &ConnectionProfile::serverChanged);
            m_ctrl->setIsServer(isServer);
            serverSpy.wait(10);
            QCOMPARE(serverSpy.count(), 1);
        }

        {
            auto port= Helper::generate(0, 20000);
            while(port == m_ctrl->port())
                port= Helper::generate(0, 20000);
            QSignalSpy portSpy(profile, &ConnectionProfile::portChanged);
            m_ctrl->setPort(port);
            portSpy.wait(10);
            QCOMPARE(portSpy.count(), 1);
        }
        if(!isServer)
        {
            auto address= "192.168.1.24";
            profile->setAddress(address);
            QCOMPARE(m_ctrl->address(), address);
        }

        for(int j= 0; j < characterCount; ++j)
        {
            m_ctrl->addCharacter();
            m_ctrl->editCharacterName(j, Helper::randomString());
            m_ctrl->editCharacterColor(j, Qt::red);
            m_ctrl->editCharacterAvatar(j, Helper::imageData(avatarSquare));
        }
    }

    for(int i= 0; i < profileCount; ++i)
    {
        m_ctrl->setCurrentProfileIndex(i);
        QCOMPARE(m_ctrl->canConnect(), expected);
    }

    for(int i= profileCount - 1; i >= 0; --i)
    {
        m_ctrl->cloneProfile(i);

        auto origin= m_profileModel->getProfile(i);
        auto clone= m_profileModel->getProfile(m_ctrl->profileModel()->rowCount() - 1);
        QVERIFY(origin->profileTitle() != clone->profileTitle());
        QCOMPARE(origin->playerName(), clone->playerName());
        QCOMPARE(origin->isGM(), clone->isGM());
        QCOMPARE(origin->isServer(), clone->isServer());
        QCOMPARE(m_ctrl->canConnect(), expected);
    }

    for(int i= m_profileModel->rowCount() - 1; i >= 0; --i)
    {
        m_ctrl->removeProfile(i);
    }
}

void ProfileControllerTest::clone_data()
{
    QTest::addColumn<int>("profileCount");
    QTest::addColumn<int>("characterCount");
    QTest::addColumn<bool>("avatarSquare");
    QTest::addColumn<bool>("isGM");
    QTest::addColumn<bool>("isServer");
    QTest::addColumn<bool>("expected");

    QTest::addRow("empty") << 0 << 0 << false << false << false << false;
    QTest::addRow("empty but gm") << 0 << 0 << false << true << false << false;
    QTest::addRow("one profile") << 1 << 0 << false << false << false << false;

    QTest::addRow("One profile but gm") << 1 << 0 << false << true << true << false;
    QTest::addRow("One profile with square") << 1 << 1 << true << true << true << true;
    QTest::addRow("One profile without square") << 1 << 1 << false << true << true << false;

    for(int i= 0; i < 5; ++i)
    {
        QTest::addRow("%d profile but gm", i) << i << 0 << false << true << true << false;
        QTest::addRow("%d profile with square", i) << i << i << true << true << true << true;
        QTest::addRow("%d profile without square", i) << i << i << false << true << true << false;
    }
}

void ProfileControllerTest::serialization()
{
    QFETCH(int, profileCount);
    QFETCH(int, characterCount);
    QFETCH(bool, avatarSquare);
    QFETCH(bool, isGM);
    QFETCH(bool, isServer);
    QFETCH(bool, expected);

    m_ctrl.reset(new SelectConnProfileController(nullptr, nullptr));

    // should not lead to crash
    m_ctrl->addProfile();
    m_ctrl->cloneProfile(0);
    m_ctrl->removeProfile(0);
    m_ctrl->validCharacterAvatar();
    m_ctrl->validCharacterColor();
    m_ctrl->validCharacterName();
    m_ctrl->address();
    m_ctrl->campaignPath();
    m_ctrl->playerAvatar();
    m_ctrl->playerColor();
    m_ctrl->playerName();

    m_profileModel.reset(new ProfileModel());

    for(int i= 0; i < profileCount; ++i)
    {
        m_profileModel->appendProfile();
        QCOMPARE(m_profileModel->rowCount(), i + 1);
        auto profile= m_profileModel->getProfile(i);
        QSignalSpy nameSpy(profile, &ConnectionProfile::titleChanged);

        auto name= Helper::randomString();
        // set player info
        profile->setProfileTitle(name);
        nameSpy.wait(10);
        QCOMPARE(nameSpy.count(), 1);
        auto playername= Helper::randomString();
        QSignalSpy playerNameSpy(profile, &ConnectionProfile::playerNameChanged);
        profile->setPlayerName(playername);
        playerNameSpy.wait(10);
        QCOMPARE(playerNameSpy.count(), 1);

        profile->setPlayerAvatar(Helper::imageData(avatarSquare));

        // set Campaign info
        QTemporaryDir dir;
        dir.setAutoRemove(false);
        dir.isValid();
        auto path= dir.path();

        if(profile->isGM() != isGM)
        {
            QSignalSpy gmSpy(profile, &ConnectionProfile::gmChanged);
            profile->setGm(isGM);
            gmSpy.wait(10);
            QCOMPARE(gmSpy.count(), 1);
        }

        QSignalSpy campSpy(profile, &ConnectionProfile::campaignPathChanged);
        profile->setCampaignPath(path);
        campSpy.wait(10);

        QCOMPARE(campSpy.count(), 1);

        // set network info
        if(profile->isServer() != isServer)
        {
            QSignalSpy serverSpy(profile, &ConnectionProfile::serverChanged);
            profile->setServerMode(isServer);
            serverSpy.wait(10);
            QCOMPARE(serverSpy.count(), 1);
        }

        {
            auto port= Helper::generate(0, 20000);
            while(port == profile->port())
                port= Helper::generate(0, 20000);
            QSignalSpy portSpy(profile, &ConnectionProfile::portChanged);
            profile->setPort(port);
            portSpy.wait(10);
            QCOMPARE(portSpy.count(), 1);
        }
        if(!isServer)
        {
            auto address= "192.168.1.24";
            profile->setAddress(address);
            m_ctrl->address();
        }

        for(int j= 0; j < characterCount; ++j)
        {
            profile->addCharacter(
                connection::CharacterData({Helper::randomString(), Qt::red, Helper::imageData(avatarSquare), {}}));
        }
    }

    m_ctrl.reset(new SelectConnProfileController(m_profileModel.get(), nullptr));

    for(int i= 0; i < profileCount; ++i)
    {
        m_ctrl->setCurrentProfileIndex(i);
        // auto prof= m_profileModel->getProfile(i);
        //         qDebug() << "characters:" << prof->charactersValid() << "connections" << prof->connectionInfoValid()
        //                << "campaignInfoValid" << prof->campaignInfoValid() << "playerInfoValid" <<
        //                prof->playerInfoValid();

        QCOMPARE(m_ctrl->canConnect(), expected);
    }

    for(int i= profileCount - 1; i >= 0; --i)
    {
        m_ctrl->cloneProfile(i);

        auto origin= m_profileModel->getProfile(i);
        auto clone= m_profileModel->getProfile(m_ctrl->profileModel()->rowCount() - 1);
        QVERIFY(origin->profileTitle() != clone->profileTitle());
        QCOMPARE(origin->playerName(), clone->playerName());
        QCOMPARE(origin->isGM(), clone->isGM());
        QCOMPARE(origin->isServer(), clone->isServer());
        QCOMPARE(m_ctrl->canConnect(), expected);
    }
}
void ProfileControllerTest::serialization_data()
{
    QTest::addColumn<int>("profileCount");
    QTest::addColumn<int>("characterCount");
    QTest::addColumn<bool>("avatarSquare");
    QTest::addColumn<bool>("isGM");
    QTest::addColumn<bool>("isServer");
    QTest::addColumn<bool>("expected");

    QTest::addRow("empty") << 0 << 0 << false << false << false << false;
    QTest::addRow("empty but gm") << 0 << 0 << false << true << false << false;
    QTest::addRow("one profile") << 1 << 0 << false << false << false << false;

    QTest::addRow("One profile but gm") << 1 << 0 << false << true << true << false;
    QTest::addRow("One profile with square") << 1 << 1 << true << true << true << true;
    QTest::addRow("One profile without square") << 1 << 1 << false << true << true << false;

    for(int i= 0; i < 5; ++i)
    {
        QTest::addRow("%d profile but gm", i) << i << 0 << false << true << true << false;
        QTest::addRow("%d profile with square", i) << i << i << true << true << true << true;
        QTest::addRow("%d profile without square", i) << i << i << false << true << true << false;
    }
}

void ProfileControllerTest::connectTest()
{
    m_ctrl.reset(new SelectConnProfileController(m_profileModel.get(), nullptr));
    m_ctrl->setCurrentProfileIndex(10);
    m_ctrl->setCurrentProfileIndex(m_ctrl->currentProfileIndex());
    QCOMPARE(m_profileModel->rowCount(), 0);

    m_profileModel.reset(new ProfileModel());
    auto profil= new ConnectionProfile();
    profil->setProfileTitle("Text1");
    m_profileModel->appendProfile(profil);
    m_ctrl.reset(new SelectConnProfileController(m_profileModel.get(), nullptr));

    auto profileModel= m_ctrl->profileModel();
    QCOMPARE(profileModel->rowCount(), 1);
    QCOMPARE(m_ctrl->profileName(), "Text1");

    QVERIFY(!m_ctrl->validCampaignPath());

    QTemporaryDir dir;
    dir.setAutoRemove(false);
    dir.isValid();
    auto path= dir.path();

    QSignalSpy campSpy(m_ctrl.get(), &SelectConnProfileController::campaignPathChanged);
    m_ctrl->setCampaignPath(path);
    campSpy.wait(10);

    QCOMPARE(campSpy.count(), 1);

    QCOMPARE(m_ctrl->connectionState(), SelectConnProfileController::ConnectionState::IDLE);

    auto error= Helper::randomString();
    m_ctrl->setErrorMsg(error);
    QCOMPARE(m_ctrl->errorMsg(), error);
    m_ctrl->setErrorMsg(error);

    auto info= Helper::randomString();
    m_ctrl->setInfoMsg(info);
    QCOMPARE(m_ctrl->infoMsg(), info);
    m_ctrl->setInfoMsg(info);

    m_ctrl->addCharacter();
    auto model= m_ctrl->characterModel();

    QCOMPARE(model->rowCount(), 1);
    auto name= Helper::randomString();
    m_ctrl->editCharacterName(0, name);
    auto character= model->character(0);
    QCOMPARE(character.m_name, name);

    m_ctrl->editCharacterColor(0, Qt::darkGray);
    character= model->character(0);
    QCOMPARE(character.m_color, Qt::darkGray);

    auto avatar= Helper::imageData(true);
    m_ctrl->editCharacterAvatar(0, avatar);
    m_ctrl->editCharacterColor(0, Qt::red);
    m_ctrl->editCharacterName(0, Helper::randomString());
    character= model->character(0);
    QCOMPARE(character.m_avatarData, avatar);
    QVERIFY(m_ctrl->validCharacter());
    QVERIFY(m_ctrl->validCharacterAvatar());
    QVERIFY(m_ctrl->validCharacterColor());
    QVERIFY(m_ctrl->validCharacterName());

    m_ctrl->removeCharacter(0);
    QCOMPARE(model->rowCount(), 0);

    auto pw= Helper::randomString();
    m_ctrl->setPassword(pw);
    QVERIFY(m_ctrl->password() != pw);
}

void ProfileControllerTest::canConnect()
{
    QFETCH(QString, profileName);
    QFETCH(QString, playerName);
    QFETCH(QColor, playerColor);
    QFETCH(QByteArray, playerAvatar);
    QFETCH(bool, isGameMaster);
    QFETCH(bool, isServer);
    QFETCH(QString, address);
    QFETCH(int, port);
    QFETCH(QString, campaignPath);
    QFETCH(QList<CharacterInfo>, characters);
    QFETCH(bool, canConnectStatus);

    QCOMPARE(m_profileModel->rowCount(), 0);
    m_ctrl->addProfile();
    QCOMPARE(m_profileModel->rowCount(), 1);

    m_ctrl->setCurrentProfileIndex(0);
    QCOMPARE(m_ctrl->currentProfileIndex(), 0);

    QSignalSpy spy(m_ctrl.get(), &SelectConnProfileController::canConnectChanged);

    m_ctrl->setProfileName(profileName);

    m_ctrl->setPlayerName(playerName);
    QCOMPARE(m_ctrl->playerName(), playerName);
    m_ctrl->setPlayerColor(playerColor);
    QCOMPARE(m_ctrl->playerColor(), playerColor);
    m_ctrl->setPlayerAvatar(playerAvatar);
    QCOMPARE(m_ctrl->playerAvatar(), playerAvatar);

    m_ctrl->setIsGameMaster(isGameMaster);
    m_ctrl->setIsServer(isServer);

    m_ctrl->setAddress(address);
    m_ctrl->setPort(port);

    m_ctrl->setCampaignPath(campaignPath);
    auto characterModel= m_ctrl->characterModel();
    characterModel->removeCharacter(0);

    QList<connection::CharacterData> list;
    std::transform(std::begin(characters), std::end(characters), std::back_inserter(list),
                   [](const CharacterInfo& info) -> connection::CharacterData {
                       return {info.name, info.color, info.avatar, {}};
                   });

    for(auto const& data : qAsConst(list))
    {
        characterModel->addCharacter(data);
    }

    spy.wait(10);

    QCOMPARE(m_ctrl->canConnect(), canConnectStatus);

    if(isGameMaster && !campaignPath.isNull())
        QVERIFY2(m_ctrl->validCampaignPath(), QString("campaignPath: %1").arg(campaignPath).toStdString().c_str());
}

void ProfileControllerTest::canConnect_data()
{
    QTest::addColumn<QString>("profileName");
    QTest::addColumn<QString>("playerName");
    QTest::addColumn<QColor>("playerColor");
    QTest::addColumn<QByteArray>("playerAvatar");
    QTest::addColumn<bool>("isGameMaster");
    QTest::addColumn<bool>("isServer");
    QTest::addColumn<QString>("address");
    QTest::addColumn<int>("port");
    QTest::addColumn<QString>("campaignPath");
    QTest::addColumn<QList<CharacterInfo>>("characters");
    QTest::addColumn<bool>("canConnectStatus");

    auto badImg= IOHelper::imageToData(IOHelper::readImageFromFile(":/img/arbre_500.jpg"));
    auto goodImg= IOHelper::imageToData(IOHelper::readImageFromFile(":/img/arbre_square_500.jpg"));
    QTemporaryDir dir;
    dir.setAutoRemove(false);
    dir.isValid();

    // No valid field
    QTest::addRow("connect1") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                              << QString() << QList<CharacterInfo>() << false;

    // One valid field
    QTest::addRow("connect2") << QString("test") << QString() << QColor() << QByteArray() << false << false << QString()
                              << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect3") << QString() << QString("test") << QColor() << QByteArray() << false << false << QString()
                              << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect4") << QString() << QString() << QColor(Qt::blue) << QByteArray() << false << false
                              << QString() << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect5") << QString() << QString() << QColor() << badImg << false << false << QString() << 0
                              << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect5") << QString() << QString() << QColor() << goodImg << false << false << QString() << 0
                              << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect6") << QString() << QString() << QColor() << QByteArray() << true << false << QString() << 0
                              << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect7") << QString() << QString() << QColor() << QByteArray() << false << true << QString() << 0
                              << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect8") << QString() << QString() << QColor() << QByteArray() << false << false
                              << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect9") << QString() << QString() << QColor() << QByteArray() << false << false << QString()
                              << 6660 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect10") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << dir.path() << QList<CharacterInfo>() << false;

    QTest::addRow("connect11") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << QString() << QList<CharacterInfo>{{QString(), QColor(), QByteArray()}} << false;

    QTest::addRow("connect12") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << QString() << QList<CharacterInfo>{{QString("Player"), QColor(), QByteArray()}}
                               << false;

    QTest::addRow("connect13") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << QString() << QList<CharacterInfo>{{QString(), QColor(Qt::blue), QByteArray()}}
                               << false;

    QTest::addRow("connect14") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << QString() << QList<CharacterInfo>{{QString(), QColor(), badImg}} << false;

    QTest::addRow("connect15") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << QString() << QList<CharacterInfo>{{QString(), QColor(), goodImg}} << false;

    // two valid fields
    QTest::addRow("connect16") << QString("test") << QString("test") << QColor() << QByteArray() << false << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect17") << QString() << QString("test") << QColor(Qt::blue) << QByteArray() << false << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect18") << QString() << QString() << QColor(Qt::blue) << goodImg << false << false << QString()
                               << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect19") << QString() << QString() << QColor(Qt::blue) << badImg << false << false << QString()
                               << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect20") << QString() << QString() << QColor() << badImg << true << false << QString() << 0
                               << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect21") << QString() << QString() << QColor() << QByteArray() << true << true << QString() << 0
                               << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect22") << QString() << QString() << QColor() << QByteArray() << false << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect23") << QString() << QString() << QColor() << QByteArray() << false << false
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect24") << QString() << QString() << QColor() << QByteArray() << false << false << QString()
                               << 6660 << dir.path() << QList<CharacterInfo>() << false;

    QTest::addRow("connect25") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), QByteArray()}} << false;
    QTest::addRow("connect26") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << dir.path() << QList<CharacterInfo>{{QString(), QColor(Qt::blue), goodImg}} << false;
    QTest::addRow("connect27") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << dir.path() << QList<CharacterInfo>{{QString(), QColor(Qt::blue), badImg}} << false;
    QTest::addRow("connect28") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << dir.path() << QList<CharacterInfo>{{QString("player"), QColor(), goodImg}} << false;
    QTest::addRow("connect29") << QString() << QString() << QColor() << QByteArray() << false << false << QString() << 0
                               << dir.path() << QList<CharacterInfo>{{QString("player"), QColor(), badImg}} << false;

    // three valid fields
    QTest::addRow("connect30") << QString("test") << QString("test") << QColor(Qt::blue) << QByteArray() << false
                               << false << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect31") << QString() << QString("test") << QColor(Qt::blue) << goodImg << false << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect32") << QString() << QString("test") << QColor(Qt::blue) << badImg << false << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect33") << QString() << QString() << QColor(Qt::blue) << goodImg << true << false << QString()
                               << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect34") << QString() << QString() << QColor(Qt::blue) << badImg << true << false << QString()
                               << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect35") << QString() << QString() << QColor() << badImg << true << true << QString() << 0
                               << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect36") << QString() << QString() << QColor() << goodImg << true << true << QString() << 0
                               << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect37") << QString() << QString() << QColor() << QByteArray() << true << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect38") << QString() << QString() << QColor() << QByteArray() << false << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect39") << QString() << QString() << QColor() << QByteArray() << false << false
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;
    QTest::addRow("connect40") << QString() << QString() << QColor() << QByteArray() << false << false << QString()
                               << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), badImg}} << false;
    QTest::addRow("connect41") << QString() << QString() << QColor() << QByteArray() << false << false << QString()
                               << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // Four valid fields
    QTest::addRow("connect42") << QString("test") << QString("test") << QColor(Qt::blue) << badImg << false << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect43") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << false << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect44") << QString() << QString("test") << QColor(Qt::blue) << goodImg << true << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect45") << QString() << QString("test") << QColor(Qt::blue) << badImg << true << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect46") << QString() << QString() << QColor(Qt::blue) << badImg << true << true << QString() << 0
                               << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect47") << QString() << QString() << QColor(Qt::blue) << goodImg << true << true << QString()
                               << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect48") << QString() << QString() << QColor() << goodImg << true << true << QString("localhost")
                               << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect49") << QString() << QString() << QColor() << badImg << true << true << QString("localhost")
                               << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect50") << QString() << QString() << QColor() << QByteArray() << true << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect51") << QString() << QString() << QColor() << QByteArray() << false << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;
    QTest::addRow("connect52") << QString() << QString() << QColor() << QByteArray() << false << false
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // five valid fields
    QTest::addRow("connect53") << QString("test") << QString("test") << QColor(Qt::blue) << badImg << true << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect54") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << true << false
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect55") << QString() << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect56") << QString() << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect57") << QString() << QString() << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect58") << QString() << QString() << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect59") << QString() << QString() << QColor() << badImg << true << true << QString("localhost")
                               << 6660 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect60") << QString() << QString() << QColor() << goodImg << true << true << QString("localhost")
                               << 6660 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect61") << QString() << QString() << QColor() << QByteArray() << true << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;
    QTest::addRow("connect62") << QString() << QString() << QColor() << QByteArray() << false << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // six valid fields
    QTest::addRow("connect63") << QString("test") << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect64") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString() << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect65") << QString() << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect66") << QString() << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect67") << QString() << QString() << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect68") << QString() << QString() << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect69") << QString() << QString() << QColor() << goodImg << true << true << QString("localhost")
                               << 6660 << dir.path() << QList<CharacterInfo>() << false;
    QTest::addRow("connect70") << QString() << QString() << QColor() << badImg << true << true << QString("localhost")
                               << 6660 << dir.path() << QList<CharacterInfo>() << false;

    QTest::addRow("connect71") << QString() << QString() << QColor() << QByteArray() << true << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // seven valid fields
    QTest::addRow("connect72") << QString("test") << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect73") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 0 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect74") << QString() << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect75") << QString() << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect76") << QString() << QString() << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;
    QTest::addRow("connect77") << QString() << QString() << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;

    QTest::addRow("connect78") << QString() << QString() << QColor() << badImg << true << true << QString("localhost")
                               << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;
    QTest::addRow("connect79") << QString() << QString() << QColor() << goodImg << true << true << QString("localhost")
                               << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // eight valid fields
    QTest::addRow("connect80") << QString("test") << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;
    QTest::addRow("connect81") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << QString() << QList<CharacterInfo>() << false;

    QTest::addRow("connect82") << QString() << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;
    QTest::addRow("connect83") << QString() << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;

    QTest::addRow("connect84") << QString() << QString() << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;
    QTest::addRow("connect85") << QString() << QString() << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // nine valid field
    QTest::addRow("connect86") << QString("test") << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << false;
    QTest::addRow("connect87") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << dir.path() << QList<CharacterInfo>() << true;

    QTest::addRow("connect88") << QString() << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;
    QTest::addRow("connect89") << QString() << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // ten valid field

    // bad img
    QTest::addRow("connect90") << QString("test") << QString("test") << QColor(Qt::blue) << badImg << true << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << false;

    // false
    QTest::addRow("connect91") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << true << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}}
                               << true; // no player with game master

    QTest::addRow("connect92") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << false << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), badImg}} << false;

    QTest::addRow("connect93") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << false << true
                               << QString("localhost") << 6660 << QString()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << true;
}
QTEST_MAIN(ProfileControllerTest);
#include "tst_profilecontroller.moc"
