/***************************************************************************
 *   Copyright (C) 2018 by Renaud Guezennec                                *
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

#include <QJsonObject>
#include <QModelIndex>
#include <QModelIndexList>
#include <QTemporaryFile>
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <memory>

#include "data/character.h"
#include "model/characterstatemodel.h"
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"
#include <QAbstractItemModelTester>

class TestCharacterStateModel : public QObject
{
    Q_OBJECT
public:
    enum Action
    {
        Top,
        Bottom,
        Up,
        Down
    };
    TestCharacterStateModel();

    void addDefaultState();
private slots:
    void init();

    void addTest();
    void addTest_data();

    void moveTest();
    void moveTest_data();
    void removeTest();

    void saveModelTest();

    void networkTest();

private:
    std::unique_ptr<CharacterStateModel> m_model;
};

TestCharacterStateModel::TestCharacterStateModel() {}

void TestCharacterStateModel::init()
{
    m_model.reset(new CharacterStateModel());
    new QAbstractItemModelTester(m_model.get());
}

void TestCharacterStateModel::addDefaultState()
{
    CharacterState state;
    state.setColor(Qt::black);
    state.setLabel(tr("Healthy"));

    CharacterState state2;
    state2.setColor(QColor(255, 100, 100));
    state2.setLabel(tr("Lightly Wounded"));

    CharacterState state3;
    state3.setColor(QColor(255, 0, 0));
    state3.setLabel(tr("Seriously injured"));

    CharacterState state4;
    state4.setColor(Qt::gray);
    state4.setLabel(tr("Dead"));

    CharacterState state5;
    state5.setColor(QColor(80, 80, 255));
    state5.setLabel(tr("Sleeping"));

    CharacterState state6;
    state6.setColor(QColor(0, 200, 0));
    state6.setLabel(tr("Bewitched"));

    m_model->appendState(std::move(state));
    m_model->appendState(std::move(state2));
    m_model->appendState(std::move(state3));
    m_model->appendState(std::move(state4));
    m_model->appendState(std::move(state5));
    m_model->appendState(std::move(state6));
}

void TestCharacterStateModel::addTest()
{
    QFETCH(QString, label);
    QFETCH(QColor, color);
    QFETCH(QString, imgPath);

    QVERIFY(m_model->rowCount() == 0);

    CharacterState state;
    state.setColor(color);
    state.setImagePath(imgPath);
    state.setLabel(label);
    m_model->appendState(std::move(state));

    QCOMPARE(m_model->rowCount(), 1);
}

void TestCharacterStateModel::addTest_data()
{
    QTest::addColumn<QString>("label");
    QTest::addColumn<QColor>("color");
    QTest::addColumn<QString>("imgPath");

    QTest::addRow("state1") << "" << QColor() << "";
    QTest::addRow("state2") << "girafe" << QColor(Qt::red) << ":/assets/img/girafe.jpg";
    QTest::addRow("state3") << "lion" << QColor(Qt::yellow) << ":/assets/img/lion.jpg";
    QTest::addRow("state4") << "blue" << QColor(Qt::blue) << "";
}

void TestCharacterStateModel::removeTest()
{
    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    m_model->clear();
    QCOMPARE(m_model->rowCount(), 0);

    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    QModelIndexList indexList;
    auto index= m_model->index(0, 0);
    indexList << index;
    m_model->deleteState(index);
    QCOMPARE(m_model->rowCount(), 5);

    m_model->clear();
    QCOMPARE(m_model->rowCount(), 0);

    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    index= m_model->index(0, 0);
    auto index1= m_model->index(1, 0);
    m_model->deleteState(index1);
    m_model->deleteState(index);
    QCOMPARE(m_model->rowCount(), 4);
}

void TestCharacterStateModel::moveTest()
{
    QFETCH(int, source);
    QFETCH(int, action);
    QFETCH(int, destination);
    QFETCH(QString, text);

    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    auto index= m_model->index(source, 0);
    switch(static_cast<Action>(action))
    {
    case Up:
        m_model->upState(index);
        break;
    case Down:
        m_model->downState(index);
        break;
    case Top:
        m_model->topState(index);
        break;
    case Bottom:
        m_model->bottomState(index);
        break;
    }
    auto const& listState= m_model->statesList();
    QCOMPARE(listState.at(destination)->label(), text);
}

void TestCharacterStateModel::moveTest_data()
{
    QTest::addColumn<int>("source");
    QTest::addColumn<int>("action");
    QTest::addColumn<int>("destination");
    QTest::addColumn<QString>("text");

    QTest::newRow("test1") << 1 << static_cast<int>(Up) << 0 << QString("Lightly Wounded");
    QTest::newRow("test2") << 2 << static_cast<int>(Down) << 3 << QString("Seriously injured");
    QTest::newRow("test3") << 5 << static_cast<int>(Top) << 0 << QString("Bewitched");
    QTest::newRow("test4") << 0 << static_cast<int>(Bottom) << 5 << QString("Healthy");
}

void TestCharacterStateModel::saveModelTest()
{
    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    QTemporaryFile file;
    QVERIFY(file.open());

    auto array= campaign::FileSerializer::statesToArray(m_model->statesList(), file.fileName());

    // campaign::FileSerializer::writeStatesIntoCampaign(file.fileName(), );

    CharacterStateModel model;
    ModelHelper::fetchCharacterStateModel(array, &model, "");

    QCOMPARE(model.rowCount(), 6);
    auto const& list= model.statesList();
    QCOMPARE(list.at(0)->label(), "Healthy");
    QCOMPARE(list.at(5)->label(), "Bewitched");
}

void TestCharacterStateModel::networkTest()
{
    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    NetworkMessageWriter msg(NetMsg::CampaignCategory, NetMsg::DiceAliasModel);
    CharacterState state;
    state.setLabel("TestState");
    state.setColor(Qt::red);
    msg.uint32(1);

    msg.uint64(0);
    msg.string32(state.label());
    msg.rgb(state.color().rgb());
    msg.pixmap(state.pixmap());
    auto data= msg.data();
    NetworkMessageReader msgReader;
    msgReader.setData(data);

    MessageHelper::fetchCharacterStatesFromNetwork(&msgReader, m_model.get());

    // m_model->processAddState(&msgReader);
    QCOMPARE(m_model->rowCount(), 1);

    // m_model->setGM(false);
    auto list= Character::getCharacterStateList();
    if(list == nullptr)
        return;
    QCOMPARE(list->size(), 1);
}

QTEST_MAIN(TestCharacterStateModel);

#include "tst_characterstatemodel.moc"
