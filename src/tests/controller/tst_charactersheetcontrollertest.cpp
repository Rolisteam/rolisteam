/***************************************************************************
 *	Copyright (C) 2020 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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

#include <QSignalSpy>
#include <QTest>

#include "controller/view_controller/charactersheetcontroller.h"
#include "model/charactermodel.h"
#include "userlist/playermodel.h"
#include <memory>

class CharacterSheetControllerTest : public QObject
{
    Q_OBJECT
public:
    CharacterSheetControllerTest()= default;

private slots:
    void init();
    void cleanupTestCase();

    void gameMasterTest();

private:
    std::unique_ptr<CharacterSheetController> m_ctrl;
    std::unique_ptr<CharacterModel> m_characterModel;
    std::unique_ptr<PlayerModel> m_playerModel;
};

void CharacterSheetControllerTest::init()
{
    m_playerModel.reset(new PlayerModel());
    m_characterModel.reset(new CharacterModel());
    m_characterModel->setSourceModel(m_playerModel.get());
    // m_characterModel.get(),
    m_ctrl.reset(new CharacterSheetController("", ""));
}

void CharacterSheetControllerTest::cleanupTestCase() {}

void CharacterSheetControllerTest::gameMasterTest()
{
    QCOMPARE(m_ctrl->gameMasterId(), QString());

    QSignalSpy spy(m_ctrl.get(), &CharacterSheetController::gameMasterIdChanged);

    m_ctrl->setGameMasterId("GameMasterID");

    QCOMPARE(m_ctrl->gameMasterId(), QString("GameMasterID"));
    QCOMPARE(spy.count(), 1);
}
QTEST_MAIN(CharacterSheetControllerTest);

#include "tst_charactersheetcontrollertest.moc"
