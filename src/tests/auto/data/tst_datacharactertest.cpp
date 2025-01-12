/***************************************************************************
 *   Copyright (C) 2011 by Renaud Guezennec                                *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   Rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtCore/QString>
#include <QTest>
#include <QSignalSpy>

#include <charactersheet/charactersheet.h>
#include <charactersheet/rolisteamimageprovider.h>
#include <data/character.h>
#include <data/characterstate.h>

#include "worker/iohelper.h"
#include <helper.h>

#define COUNT_TURN 2000

CharacterAction* buildAction()
{
    auto res= new CharacterAction;
    res->setCommand(Helper::randomString());
    res->setName(Helper::randomString());
    return res;
}
CharacterProperty* buildProperty()
{
    auto res= new CharacterProperty;
    res->setValue(Helper::randomString());
    res->setName(Helper::randomString());
    return res;
}
CharacterShape* buildShape()
{
    auto res= new CharacterShape;
    res->setSize(Helper::generate<int>(1, 12));
    res->setImage(QImage(Helper::imagePath()));
    res->setUri(Helper::imagePath());
    res->setName(Helper::randomString());
    return res;
}

class DataCharacterTest : public QObject
{
    Q_OBJECT

public:
    DataCharacterTest();

private Q_SLOTS:
    void testSetAndGet();
    void testProperty();
    void initTestCase();
    void cleanupTestCase();
    void testCharacterAction();
    void testCharacterShape();
    void testCharacterProperty();

    void propertiesTest();

private:
    std::unique_ptr<Character> m_character;
};

DataCharacterTest::DataCharacterTest() {}
void DataCharacterTest::initTestCase()
{
    m_character.reset(new Character());
}

void DataCharacterTest::cleanupTestCase() {}
void DataCharacterTest::testSetAndGet()
{
    m_character->setName("Name");
    QVERIFY2(m_character->name() == "Name", "names are different");

    m_character->setAvatar(IOHelper::imageToData(QImage(":/img/girafe.jpg")));
    QVERIFY2(m_character->hasAvatar(), "has no avatar");

    m_character->setColor(Qt::red);
    QVERIFY2(m_character->getColor() == Qt::red, "not the right color");

    m_character->setNpc(true);
    QVERIFY(m_character->isNpc());

    m_character->setNpc(false);
    QVERIFY(!m_character->isNpc());

    m_character->setNumber(10);
    QVERIFY(m_character->number() == 10);

    Character cart2(Helper::randomString(), Helper::randomString(), Helper::randomColor(), false, 0);

    QList<CharacterAction*> actions;
    for(int i= Helper::generate<int>(1, 10); i >= 0; --i)
    {
        actions << buildAction();
    }
    m_character->defineActionList(actions);

    QList<CharacterProperty*> properties;
    for(int i= Helper::generate<int>(1, 10); i >= 0; --i)
    {
        properties << buildProperty();
    }
    m_character->definePropertiesList(properties);

    QList<CharacterShape*> shapes;
    for(int i= Helper::generate<int>(1, 10); i >= 0; --i)
    {
        shapes << buildShape();
    }
    m_character->defineShapeList(shapes);

    m_character->setImageProvider(nullptr);
    QCOMPARE(m_character->getImageProvider(), nullptr);

    auto p= new RolisteamImageProvider(nullptr);
    m_character->setImageProvider(p);
    QCOMPARE(m_character->getImageProvider(), p);

    m_character->setDistancePerTurn(1.0);
    m_character->setDistancePerTurn(1.0);

    QCOMPARE(m_character->getParentId(), QString());

    auto vs= m_character->getVariableDictionnary();
    QCOMPARE(vs.size(), properties.size());

    auto sheet= m_character->getSheet();
    QVERIFY(sheet == nullptr);

    m_character->setSheet(new CharacterSheet());
    auto vs2= m_character->getVariableDictionnary();
    QCOMPARE(vs2.size(), 0);

    m_character->setSheet(sheet);

    m_character->addShape(Helper::randomString(), Helper::imagePath());
    m_character->addShape(Helper::randomString(), Helper::randomUrl().toString());
    QVERIFY(m_character->currentStateLabel().isEmpty());
    QVERIFY(m_character->currentStateImage().isNull());

    m_character->setCurrentShape(0);
    m_character->setCurrentShape(800);
    m_character->setCurrentShape(2);
    m_character->setCurrentShape(2);
    m_character->setCurrentShape(nullptr);
    m_character->setCurrentShape(nullptr);
    QCOMPARE(m_character->currentShape(), nullptr);
    auto s= buildShape();

    m_character->addShape(s);
    m_character->setCurrentShape(s);
    QCOMPARE(m_character->currentShape(), s);

    m_character->addShape(nullptr);

    m_character->addProperty(buildProperty());
    m_character->addProperty(nullptr);
}
void DataCharacterTest::testProperty()
{
    QSignalSpy spy(m_character.get(), &Character::currentHealthPointsChanged);
    m_character->setHealthPointsCurrent(10);
    QVERIFY(spy.count() == 1);

    QSignalSpy spyMaxHP(m_character.get(), &Character::maxHPChanged);
    m_character->setHealthPointsMax(200);
    QVERIFY(spyMaxHP.count() == 1);

    QSignalSpy spyminHP(m_character.get(), &Character::minHPChanged);
    m_character->setHealthPointsMin(50);
    QVERIFY(spyminHP.count() == 1);

    QSignalSpy spydistancePerTurn(m_character.get(), &Character::distancePerTurnChanged);
    m_character->setDistancePerTurn(25.9);
    QVERIFY(spydistancePerTurn.count() == 1);

    QSignalSpy spyInit(m_character.get(), &Character::initiativeChanged);
    m_character->setInitiativeScore(24);
    QVERIFY(spyInit.count() == 1);

    // auto state2= new CharacterState();

    /*m_character->setState(state2);
    QSignalSpy spyState(m_character, &Character::stateChanged);
    auto state= new CharacterState();
    state->setLabel("test");
    state->setColor(Qt::red);
    state->setIsLocal(true);
    m_character->setState(state);
    QVERIFY(spyState.count() == 1);
    QVERIFY(m_character->getState() == state);
    m_character->setState(state);
    QVERIFY(spyState.count() == 1);

    m_character->setAvatarPath("/home/rolisteam");
    QSignalSpy spyAvatar(m_character, &Character::avatarPathChanged);
    m_character->setAvatarPath("/dev/null");
    QVERIFY(spyAvatar.count() == 1);
    m_character->setAvatarPath("/dev/null");
    QVERIFY(spyAvatar.count() == 1);

    m_character->setNpc(false);
    QSignalSpy spyIsNpc(m_character, &Character::npcChanged);
    m_character->setNpc(true);
    QVERIFY(spyIsNpc.count() == 1);
    m_character->setNpc(true);
    QVERIFY(spyIsNpc.count() == 1);

    m_character->setLifeColor(Qt::green);
    QSignalSpy spyColor(m_character, &Character::lifeColorChanged);
    m_character->setLifeColor(Qt::red);
    QVERIFY(spyColor.count() == 1);
    m_character->setLifeColor(Qt::red);
    QVERIFY(spyColor.count() == 1);

    m_character->setInitCommand("");
    QSignalSpy spyInitCommand(m_character, &Character::initCommandChanged);
    m_character->setInitCommand("8G4");
    QVERIFY(spyColor.count() == 1);
    m_character->setInitCommand("8G4");
    QVERIFY(spyColor.count() == 1);*/
}

void DataCharacterTest::testCharacterAction()
{
    CharacterAction action;

    QString command("8G4");
    QString name("name");
    QString name2("name2");
    QString command2("9G4");

    action.setCommand(command);
    QVERIFY(action.command() == command);

    action.setName(name);
    QVERIFY(action.name() == name);

    QVERIFY(action.getType() == CharacterField::Action);

    QVERIFY(action.getData(0, Qt::DisplayRole) == name);
    QVERIFY(action.getData(1, Qt::DisplayRole) == command);
    QVERIFY(!action.getData(1, Qt::DecorationRole).isValid());

    action.setData(0, name2, Qt::DisplayRole);
    action.setData(1, command2, Qt::DisplayRole);

    QVERIFY(action.name() == name2);
    QVERIFY(action.command() == command2);
}
void DataCharacterTest::testCharacterShape()
{
    CharacterShape shape;

    QString uri(":/assets/img/girafe.jpg");
    QString name("name");
    QString name2("name2");
    QString uri2(":/assets/img/lion.jpg");
    QImage img(":/assets/img/girafe.jpg");
    QImage img2(":/assets/img/lion.jpg");

    shape.setUri(uri);
    QVERIFY(shape.uri() == uri);

    shape.setName(name);
    QVERIFY(shape.name() == name);

    shape.setImage(img);
    QVERIFY(shape.image() == img);

    QVERIFY(shape.getType() == CharacterField::Shape);

    QVERIFY(shape.getData(0, Qt::DisplayRole) == name);
    QVERIFY(shape.getData(1, Qt::DisplayRole) == uri);

    // QVERIFY(shape.getData(0,Qt::DecorationRole) == QPixmap::fromImage(img).scaled(64,64,Qt::KeepAspectRatio));

    shape.setData(0, name2, Qt::DisplayRole);
    shape.setData(1, uri2, Qt::DisplayRole);

    QVERIFY(shape.name() == name2);
    QVERIFY(shape.uri() == uri2);
}
void DataCharacterTest::testCharacterProperty()
{
    CharacterProperty property;

    QString value("4");
    QString name("name");
    QString name2("name2");
    QString value2("5");

    property.setValue(value);
    QVERIFY(property.value() == value);

    property.setName(name);
    QVERIFY(property.name() == name);

    QVERIFY(property.getType() == CharacterField::Property);

    QVERIFY(property.getData(0, Qt::DisplayRole) == name);
    QVERIFY(property.getData(1, Qt::DisplayRole) == value);
    QVERIFY(!property.getData(1, Qt::DecorationRole).isValid());

    property.setData(0, name2, Qt::DisplayRole);
    property.setData(1, value2, Qt::DisplayRole);

    QVERIFY(property.name() == name2);
    QVERIFY(property.value() == value2);
}

void DataCharacterTest::propertiesTest()
{
    {
        CharacterShape shape;
        auto movie= Helper::randomMovie();
        shape.setMovie(movie);

        QCOMPARE(shape.movie().fileName(), movie.fileName());

        auto uri= Helper::randomString();
        shape.setUri(uri);
        shape.setUri(uri);

        CharacterShape shape2;
        shape2.setUri(Helper::imagePath());
        QVERIFY(!shape2.getData(0, Qt::DecorationRole).value<QImage>().isNull());

        QVERIFY(shape2.getData(0, Qt::BackgroundRole).isNull());

        shape2.setSize(10);
        shape2.setSize(10);
        shape2.setSize(12);
    }
}

QTEST_MAIN(DataCharacterTest);

#include "tst_datacharactertest.moc"
