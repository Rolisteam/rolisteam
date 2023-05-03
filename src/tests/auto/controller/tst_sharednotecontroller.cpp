#include <QTest>

#include <QSignalSpy>

#include "data/player.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "model/playermodel.h"
#include <QAbstractItemModelTester>
#include <helper.h>
#include <limits>


class SharedNoteControllerTest : public QObject
{
    Q_OBJECT
public:
    SharedNoteControllerTest() = default;

private slots:
    void init();

    void setAndGetTest();

private:
    std::unique_ptr<SharedNoteController> m_ctrl;
    std::unique_ptr<PlayerModel> m_playerModel;
};


void SharedNoteControllerTest::init()
{
    m_playerModel.reset(new PlayerModel);
    SharedNoteController::setPlayerModel(m_playerModel.get());
    m_ctrl.reset(new SharedNoteController);
}

void SharedNoteControllerTest::setAndGetTest()
{
    QSignalSpy spy(m_ctrl.get(), &SharedNoteController::permissionChanged);

    auto id = Helper::randomString();
    m_ctrl->setLocalId(id);
    m_ctrl->setOwnerId(id);

    m_ctrl->setPermission(ParticipantModel::hidden);
    m_ctrl->setPermission(ParticipantModel::readOnly);
    m_ctrl->setPermission(ParticipantModel::readWrite);

    QVERIFY(!m_ctrl->localCanWrite());

    auto length = Helper::generate<int>(1,100);
    m_ctrl->setUpdateCmd(Helper::randomString(length));
    m_ctrl->setUpdateCmd(QString("doc:%1").arg(Helper::randomString(length)));
    auto cmd = QString("doc:%1 %2 %3 %4").arg(Helper::generate<int>(1,100)).arg(Helper::generate<int>(1,100)).arg(length).arg(Helper::randomString(length));
    m_ctrl->setUpdateCmd(cmd);

    QCOMPARE(m_ctrl->updateCmd(), cmd);

    auto text = Helper::randomString();
    m_ctrl->setTextUpdate(text);
    m_ctrl->setTextUpdate(text);

    m_ctrl->setText(text);
    m_ctrl->setText(text);

    QVERIFY(!m_ctrl->canRead(nullptr));
    QVERIFY(!m_ctrl->canWrite(nullptr));

    {
        auto model = m_ctrl->playerModel();
        QCOMPARE(model, m_playerModel.get());
    }
    m_ctrl->setUrl(Helper::randomUrl());

    // MODEL Player
    PlayerModel model;
    SharedNoteController::setPlayerModel(&model);
    m_ctrl.reset(new SharedNoteController(id,id));//local is owner

    Player* local = new Player;
    local->setUuid(id);
    model.setLocalPlayerId(id);
    model.addPlayer(local);


    Player* p2 = new Player;
    p2->setUuid(Helper::randomString());
    model.addPlayer(p2);

    m_ctrl->setPermission(ParticipantModel::hidden);
    m_ctrl->setPermission(ParticipantModel::readOnly);
    m_ctrl->setPermission(ParticipantModel::readWrite);

    m_ctrl->promoteCurrentItem(QModelIndex());
    m_ctrl->demoteCurrentItem(QModelIndex());
}


QTEST_MAIN(SharedNoteControllerTest);
#include "tst_sharednotecontroller.moc"
