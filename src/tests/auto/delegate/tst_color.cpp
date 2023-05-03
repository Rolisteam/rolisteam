/***************************************************************************
 *   Copyright (C) 2022 by Renaud Guezennec                                *
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

#include <QStandardItemModel>

#include "rwidgets/delegates/colordelegate.h"
#include "rwidgets/customs/colorlisteditor.h"
#include <memory>

class ColorDelegateTest : public QObject
{
    Q_OBJECT

public:
    ColorDelegateTest();

private slots:
    void init();
    void cleanupTestCase();
    void usageTest();

private:
    std::unique_ptr<rwidgets::ColorDelegate> m_delegate;
};

ColorDelegateTest::ColorDelegateTest() {}

void ColorDelegateTest::init()
{
    m_delegate.reset(new rwidgets::ColorDelegate());
}

void ColorDelegateTest::cleanupTestCase() {}

void ColorDelegateTest::usageTest()
{
    QStandardItemModel model;
    auto a = new QStandardItem;
    a->setData(QVariant::fromValue(Qt::black), Qt::DisplayRole);
    model.appendRow(a);

    auto b = new QStandardItem;
    b->setData(QVariant::fromValue(Qt::green), Qt::DisplayRole);
    model.appendRow(b);

    auto editor = dynamic_cast<rwidgets::ColorListEditor*>(m_delegate->createEditor(nullptr, QStyleOptionViewItem(), QModelIndex()));

    m_delegate->setEditorData(editor, model.index(0,1));

    editor->setColor(Qt::darkMagenta);
    m_delegate->setModelData(editor, &model, model.index(0,1));


}

QTEST_MAIN(ColorDelegateTest);

#include "tst_color.moc"
