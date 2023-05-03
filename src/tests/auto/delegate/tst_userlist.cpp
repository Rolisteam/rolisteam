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
#include "rwidgets/delegates/userlistdelegate.h"
#include <QColor>
#include <QRectF>
#include <QSignalSpy>
#include <QTest>
#include <helper.h>

#include <map>
#include <memory>
#include <vector>

class UserListTest : public QObject
{
    Q_OBJECT
public:
    UserListTest()
    {
    };

private slots:
    void init();
    void cleanupTestCase();
private:
    std::unique_ptr<UserListDelegate> m_delegate;
};

void UserListTest::init()
{
    m_delegate.reset(new UserListDelegate);
}

void UserListTest::cleanupTestCase()
{

}


QTEST_MAIN(UserListTest);

#include "tst_userlist.moc"
