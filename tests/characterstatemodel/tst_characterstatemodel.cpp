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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QModelIndex>
#include <QModelIndexList>
#include <memory>
#include <QJsonObject>

#include "preferences/characterstatemodel.h"
#include "network/networkmessagewriter.h"
#include "network/networkmessagereader.h"
#include "data/character.h"

class TestCharacterStateModel : public QObject
{
    Q_OBJECT
public:
    TestCharacterStateModel();

    void addDefaultState();
private slots:
    void init();

    void addTest();
    void addTest_data();

    void moveTest();
    void removeTest();

    void saveModelTest();

    void networkTest();

private:
    std::unique_ptr<CharacterStateModel> m_model;
};

TestCharacterStateModel::TestCharacterStateModel()
{

}

void TestCharacterStateModel::init()
{
    m_model.reset(new CharacterStateModel());
}

void TestCharacterStateModel::addDefaultState()
{
    CharacterState* state = new CharacterState();
    state->setColor(Qt::black);
    state->setLabel(tr("Healthy"));
    m_model->addState(state);

    state = new CharacterState();
    state->setColor(QColor(255, 100, 100));
    state->setLabel(tr("Lightly Wounded"));
    m_model->addState(state);

    state = new CharacterState();
    state->setColor(QColor(255, 0, 0));
    state->setLabel(tr("Seriously injured"));
    m_model->addState(state);

    state = new CharacterState();
    state->setColor(Qt::gray);
    state->setLabel(tr("Dead"));
    m_model->addState(state);

    state = new CharacterState();
    state->setColor(QColor(80, 80, 255));
    state->setLabel(tr("Sleeping"));
    m_model->addState(state);

    state = new CharacterState();
    state->setColor(QColor(0, 200, 0));
    state->setLabel(tr("Bewitched"));
    m_model->addState(state);
}

void TestCharacterStateModel::addTest()
{
    QFETCH(QString, label);
    QFETCH(QColor, color);
    QFETCH(QString, imgPath);


    QVERIFY(m_model->rowCount() == 0);

    auto state = new CharacterState();
    state->setColor(color);
    state->setImage(QPixmap(imgPath));
    state->setLabel(label);
    m_model->addState(state);

    QCOMPARE(m_model->rowCount(), 1);
}

void TestCharacterStateModel::addTest_data()
{
   QTest::addColumn<QString>("label");
   QTest::addColumn<QColor>("color");
   QTest::addColumn<QString>("imgPath");


   QTest::addRow("state1") <<  "" << QColor() << "" ;
   QTest::addRow("state2") <<  "girafe" << QColor(Qt::red) << ":/assets/img/girafe.jpg" ;
   QTest::addRow("state3") <<  "lion" << QColor(Qt::yellow) << ":/assets/img/lion.jpg" ;
   QTest::addRow("state4") <<  "blue" << QColor(Qt::blue) << "" ;
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
    auto index = m_model->index(0, 0);
    indexList << index;
    m_model->deleteState(index);
    QCOMPARE(m_model->rowCount(), 5);

    m_model->clear();
    QCOMPARE(m_model->rowCount(), 0);

    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    index = m_model->index(0,0);
    auto index1 = m_model->index(1,0);
    m_model->deleteState(index1);
    m_model->deleteState(index);
    QCOMPARE(m_model->rowCount(), 4);
}

void TestCharacterStateModel::moveTest()
{
    auto listState = m_model->getCharacterStates();

    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    auto index = m_model->index(1,0);
    m_model->upState(index);
    QCOMPARE(listState->at(0)->getLabel(),"Lightly Wounded");

    index = m_model->index(1,0);
    m_model->downState(index);
    QCOMPARE(listState->at(1)->getLabel(),"Seriously injured");
    QCOMPARE(listState->at(2)->getLabel(),"Healthy");

    index = m_model->index(5,0);
    m_model->topState(index);
    QCOMPARE(listState->at(0)->getLabel(),"Bewitched");

    index = m_model->index(0,0);
    m_model->bottomState(index);
    QCOMPARE(listState->at(5)->getLabel(),"Bewitched");
}

void TestCharacterStateModel::saveModelTest()
{
    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    QJsonObject obj;
    m_model->save(obj);

    CharacterStateModel model;

    model.load(obj);
    QCOMPARE(model.rowCount(), 6);
    auto list = model.getCharacterStates();
    QCOMPARE(list->at(0)->getLabel(), "Healthy");
    QCOMPARE(list->at(5)->getLabel(), "Bewitched");

}

void TestCharacterStateModel::networkTest()
{
    addDefaultState();
    QCOMPARE(m_model->rowCount(), 6);

    NetworkMessageWriter  msg(NetMsg::SharePreferencesCategory, NetMsg::addState);
    CharacterState state;
    state.setLabel("TestState");
    state.setColor(Qt::red);

    msg.uint64(0);
    msg.string32(state.getLabel());
    msg.rgb(state.getColor().rgb());
    msg.uint8(state.hasImage());
    if(state.hasImage())
    {
        auto img = state.getImage();
        QByteArray array;
        QBuffer buffer(&array);
        img.save(&buffer,"PNG");
        msg.byteArray32(array);
    }
    auto data = msg.getData();
    NetworkMessageReader msgReader;
    msgReader.setData(data);


    m_model->processAddState(&msgReader);
    QCOMPARE(m_model->rowCount(), 6);

    m_model->setGM(false);
    auto list = Character::getCharacterStateList();
    QCOMPARE(list->size(), 1);
}

QTEST_MAIN(TestCharacterStateModel);

#include "tst_characterstatemodel.moc"
