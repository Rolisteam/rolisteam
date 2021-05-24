/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   Copyright (C) 2015 by Benoit Lagarde                                  *
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
#include <QtTest/QtTest>

#include <QDir>
#include <QFileInfo>
#include <QTemporaryDir>

#include "data/campaign.h"
#include "data/campaigneditor.h"
#include "data/campaignmanager.h"
#include "worker/fileserializer.h"
#include "worker/iohelper.h"
#include "worker/modelhelper.h"

class CampaignTest : public QObject
{
    Q_OBJECT

public:
    CampaignTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void createCampaign();

private:
    std::unique_ptr<campaign::CampaignManager> m_manager;
};
CampaignTest::CampaignTest() {}
void CampaignTest::initTestCase() {}

void CampaignTest::cleanupTestCase() {}

void CampaignTest::createCampaign()
{
    QTemporaryDir dir;
    if(dir.isValid())
    {
        auto path= dir.path();
        m_manager->createCampaign(path);
        QVERIFY(QFileInfo::exists(path + "/" + campaign::TRASH_FOLDER));
        QVERIFY(QFileInfo::exists(path + "/" + campaign::MEDIA_ROOT));
        QVERIFY(QFileInfo::exists(path + "/" + campaign::STATE_ROOT));
    }
}

QTEST_MAIN(CampaignTest);

#include "tst_campaign.moc"
