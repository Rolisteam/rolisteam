#include <QTest>

#include <QSignalSpy>

#include "core/controller/gamecontroller.h"
#include "core/controller/view_controller/selectconnprofilecontroller.h"
#include "core/model/profilemodel.h"
#include "core/network/characterdatamodel.h"
#include "core/network/connectionprofile.h"
#include "core/worker/iohelper.h"

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
    void cleanupTestCase();

    void canConnect();
    void canConnect_data();

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
}
void ProfileControllerTest::cleanupTestCase() {}
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

    m_ctrl->setProfileName(profileName);

    m_ctrl->setPlayerName(playerName);
    m_ctrl->setPlayerColor(playerColor);
    m_ctrl->setPlayerAvatar(playerAvatar);

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

    QSignalSpy spy(m_ctrl.get(), &SelectConnProfileController::isWorkingChanged);
    spy.wait(100);
    QCOMPARE(m_ctrl->isWorking(), false);

    for(auto const& data : qAsConst(list))
    {
        characterModel->addCharacter(data);
    }

    spy.wait(500);

    QVERIFY(spy.count() > 0);
    while(m_ctrl->isWorking())
        spy.wait(500);

    QCOMPARE(m_ctrl->canConnect(), canConnectStatus);
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
                               << false; // no player with game master

    QTest::addRow("connect92") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << false << true
                               << QString("localhost") << 6660 << dir.path()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), badImg}} << false;

    QTest::addRow("connect93") << QString("test") << QString("test") << QColor(Qt::blue) << goodImg << false << true
                               << QString("localhost") << 6660 << QString()
                               << QList<CharacterInfo>{{QString("player"), QColor(Qt::blue), goodImg}} << true;
}
QTEST_MAIN(ProfileControllerTest);
#include "tst_profilecontroller.moc"
