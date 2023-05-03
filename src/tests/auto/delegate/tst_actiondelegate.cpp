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

#include "rwidgets/delegates/actiondelegate.h"
#include <helper.h>
#include <QAbstractItemModelTester>
#include <memory>

class ActionDelegateTest : public QObject
{
    Q_OBJECT
public:
    ActionDelegateTest()= default;

private slots:
    void init();
    void setAndGetTest();


private:
    std::unique_ptr<ActionDelegate> m_delegate;
    std::unique_ptr<LabelWithOptions> m_label;
};

void ActionDelegateTest::init()
{
    m_label.reset(new LabelWithOptions);
    m_delegate.reset(new ActionDelegate);
}


void ActionDelegateTest::setAndGetTest()
{
    {
        QSignalSpy spy2(m_label.get(), &LabelWithOptions::textChanged);

        m_label->setData({Helper::randomString(),Helper::randomString(),Helper::randomString(),Helper::randomString(),Helper::randomString()},
                         {Helper::randomString(),Helper::randomString(),Helper::randomString(),Helper::randomString(),Helper::randomString()});

        auto txt = Helper::randomString();
        m_label->setText(txt);
        m_label->setText(txt);
        QCOMPARE(m_label->text(), txt);
        QCOMPARE(spy2.count(), 1);
    }



}


QTEST_MAIN(ActionDelegate);

#include "tst_actiondelegate.moc"
