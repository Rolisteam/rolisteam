
#include "data/character.h"
#include "data/localpersonmodel.h"
#include "data/player.h"
#include "model/charactermodel.h"
#include "model/playermodel.h"
#include <QAbstractItemModelTester>
#include <QTest>
#include <memory>

class CharacterModelTest : public QObject
{
    Q_OBJECT
public:
    CharacterModelTest();

private slots:
    void init();
    void addTest();
    void addTest_data();

    void nameTest();
    void nameTest_data();

    void localPersonTest();
    void localPersonTest_data();

private:
    std::unique_ptr<PlayerModel> m_playerModel;
    std::unique_ptr<CharacterModel> m_characterModel;
    std::unique_ptr<LocalPersonModel> m_localPersonModel;
};

void CharacterModelTest::init()
{
    m_playerModel.reset(new PlayerModel());
    m_characterModel.reset(new CharacterModel());
    m_localPersonModel.reset(new LocalPersonModel());

    new QAbstractItemModelTester(m_playerModel.get());

    m_characterModel->setSourceModel(m_playerModel.get());
    m_localPersonModel->setSourceModel(m_playerModel.get());
    new QAbstractItemModelTester(m_characterModel.get());
    new QAbstractItemModelTester(m_localPersonModel.get());
}

CharacterModelTest::CharacterModelTest()= default;

void CharacterModelTest::addTest()
{
    QFETCH(int, count);
    QFETCH(QVector<Player*>, players);

    m_playerModel->clear();
    QVERIFY(m_playerModel->rowCount() == 0);

    for(auto p : players)
        m_playerModel->addPlayer(p);

    QCOMPARE(m_characterModel->rowCount(), count);
}

void CharacterModelTest::addTest_data()
{
    QTest::addColumn<int>("count");
    QTest::addColumn<QVector<Player*>>("players");

    QTest::addRow("cmd 1") << 0 << QVector<Player*>();

    {
        QVector<Player*> players;
        players.push_back(new Player("name", QColor(Qt::red), true));
        QTest::addRow("cmd 2") << 0 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter("bb", "character", QColor(Qt::blue), "", QHash<QString, QVariant>(), false);
        players.push_back(player);
        QTest::addRow("cmd 3") << 1 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter(new Character("character", QColor(Qt::blue), true));
        players.push_back(player);
        QTest::addRow("cmd 4") << 1 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter(new Character("character", QColor(Qt::blue), false));
        player->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player);
        QTest::addRow("cmd 5") << 2 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter(new Character("character", QColor(Qt::blue), false));
        players.push_back(player);

        auto player2= new Player("name2", QColor(Qt::magenta), true);
        player2->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player2);

        QTest::addRow("cmd 6") << 2 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        players.push_back(player);

        auto player2= new Player("name2", QColor(Qt::magenta), true);
        player2->addCharacter(new Character("character", QColor(Qt::blue), false));
        player2->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player2);
        QTest::addRow("cmd 7") << 2 << players;
    }
}

void CharacterModelTest::nameTest()
{
    QFETCH(QString, name);
    QFETCH(QString, uuid);
    QFETCH(int, count);
    QFETCH(QVector<Player*>, players);

    m_playerModel->clear();
    QVERIFY(m_playerModel->rowCount() == 0);

    for(auto p : players)
        m_playerModel->addPlayer(p);

    QCOMPARE(m_characterModel->rowCount(), count);

    if(count == 0)
        return;

    auto index= m_characterModel->index(0, 0, QModelIndex());

    QCOMPARE(index.data(PlayerModel::NameRole).toString(), name);
    QCOMPARE(index.data(PlayerModel::IdentifierRole).toString(), uuid);
}

void CharacterModelTest::nameTest_data()
{
    QTest::addColumn<QString>("uuid");
    QTest::addColumn<QString>("name");
    QTest::addColumn<int>("count");
    QTest::addColumn<QVector<Player*>>("players");

    QTest::addRow("cmd 1") << QString() << QString() << 0 << QVector<Player*>();

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        auto character= new Character("character", QColor(Qt::blue), true);
        character->setUuid("character0");
        player->addCharacter(character);
        players.push_back(player);
        QTest::addRow("cmd 3") << "character0"
                               << "character" << 1 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        auto character= new Character("character", QColor(Qt::blue), true);
        character->setUuid("character0");
        player->addCharacter(character);
        players.push_back(player);
        QTest::addRow("cmd 4") << "character0"
                               << "character" << 1 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        auto character= new Character("character", QColor(Qt::blue), true);
        character->setUuid("character0");
        player->addCharacter(character);
        player->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player);
        QTest::addRow("cmd 5") << "character0"
                               << "character" << 2 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        auto character= new Character("character", QColor(Qt::blue), true);
        character->setUuid("character0");
        player->addCharacter(character);
        players.push_back(player);

        auto player2= new Player("name2", QColor(Qt::magenta), true);
        player2->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player2);

        QTest::addRow("cmd 6") << "character0"
                               << "character" << 2 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        players.push_back(player);

        auto player2= new Player("name2", QColor(Qt::magenta), true);
        auto character= new Character("character", QColor(Qt::blue), true);
        character->setUuid("character0");
        player2->addCharacter(character);
        player2->addCharacter(new Character("character2", QColor(Qt::green), false));
        players.push_back(player2);
        QTest::addRow("cmd 7") << "character0"
                               << "character" << 2 << players;
    }
}

void CharacterModelTest::localPersonTest()
{
    QFETCH(int, count);
    QFETCH(QVector<Player*>, players);

    m_playerModel->clear();
    QVERIFY(m_playerModel->rowCount() == 0);

    for(auto p : players)
        m_playerModel->addPlayer(p);

    QCOMPARE(m_localPersonModel->rowCount(), count);
}

void CharacterModelTest::localPersonTest_data()
{
    QTest::addColumn<int>("count");
    QTest::addColumn<QVector<Player*>>("players");

    QTest::addRow("cmd 1") << 0 << QVector<Player*>();

    {
        QVector<Player*> players;
        players.push_back(new Player("name", QColor(Qt::red), true));
        QTest::addRow("cmd 2") << 1 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter("nn", "character", QColor(Qt::blue), "", QHash<QString, QVariant>(), false);
        players.push_back(player);
        QTest::addRow("cmd 3") << 2 << players;
    }

    {
        QVector<Player*> players;
        auto player= new Player("name", QColor(Qt::red), true);
        player->addCharacter("aa", "character", QColor(Qt::blue), "", QHash<QString, QVariant>(), false);
        players.push_back(player);

        player= new Player("name2", QColor(Qt::red), true);
        players.push_back(player);
        QTest::addRow("cmd 3") << 2 << players;
    }
}

QTEST_MAIN(CharacterModelTest)

#include "tst_charactermodel.moc"
