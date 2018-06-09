/***************************************************************************
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

#include <palettemodel.h>

class PaletteModelTest : public QObject
{
    Q_OBJECT

public:
    PaletteModelTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void getAndSetTest();
    void paletteColorChangedTest();
    void paletteColorChangedTest2();
    void paletteColorChangedTest3();
    void paletteColorChangedTest4();
    void getAndSetColorTest();
    void getAndSetColorNameTest();
    void paletteColorConstructor();


private:
    PaletteModel* m_paletteModel;
    PaletteColor* m_paletteColor;
};
PaletteModelTest::PaletteModelTest()
{

}
void PaletteModelTest::initTestCase()
{
    m_paletteModel=new PaletteModel(this);
    m_paletteColor=new PaletteColor(QColor(),"WindowText",QPalette::Active,QPalette::WindowText);
}

void PaletteModelTest::getAndSetTest()
{
    QPalette  test_palette;
    m_paletteModel->setPalette(test_palette);
    QPalette result=m_paletteModel->getPalette();

    bool equal = true;
    for(int grp = 0; grp <  QPalette::NColorGroups; grp++)
    {
        for(int role = 0; role < QPalette::NColorRoles; role++)
        {
             if((result.color((QPalette::ColorGroup)grp,(QPalette::ColorRole)role) != test_palette.color((QPalette::ColorGroup)grp,(QPalette::ColorRole)role)))
             {
                 equal = false;
             }
        }
    }
    QVERIFY(equal);
}
void PaletteModelTest::paletteColorChangedTest()
{
    QPalette  test_palette;
    m_paletteModel->setPalette(test_palette);
    QColor test_color(1,87,42);
    QModelIndex firstIndex=m_paletteModel->index(0,0);
    m_paletteModel->setColor(firstIndex,test_color);
    QColor result_color=m_paletteModel->data(firstIndex,Qt::DecorationRole).value<QColor>();
    QVERIFY(result_color==test_color);
}
void PaletteModelTest::paletteColorChangedTest2()
{
    QPalette  test_palette;
    m_paletteModel->setPalette(test_palette);
    QColor test_color(1,87,42);
    QModelIndex firstIndex=m_paletteModel->index(10,0);
    m_paletteModel->setColor(firstIndex,test_color);
    QColor result_color=m_paletteModel->data(firstIndex,Qt::DecorationRole).value<QColor>();
    QVERIFY(result_color==test_color);
}
void PaletteModelTest::paletteColorChangedTest3()
{
    QPalette  test_palette;
    m_paletteModel->setPalette(test_palette);
    QColor test_color(201,7,252);
    QModelIndex firstIndex=m_paletteModel->index(20,0);
    m_paletteModel->setColor(firstIndex,test_color);
    QColor result_color=m_paletteModel->data(firstIndex,Qt::DecorationRole).value<QColor>();
    QVERIFY(result_color==test_color);
}
void PaletteModelTest::paletteColorChangedTest4()
{
    QPalette  test_palette;
    m_paletteModel->setPalette(test_palette);
    QColor test_color(102,87,42);
    QModelIndex firstIndex=m_paletteModel->index(30,0);
    m_paletteModel->setColor(firstIndex,test_color);
    QColor result_color=m_paletteModel->data(firstIndex,Qt::DecorationRole).value<QColor>();
    QVERIFY(result_color==test_color);
}
void PaletteModelTest::getAndSetColorTest()
{
    QColor  test_color(1,2,3);
    m_paletteColor->setColor(test_color);
    QColor result=m_paletteColor->getColor();
    QVERIFY(result==test_color);
}
void PaletteModelTest::getAndSetColorNameTest()
{
    QString  test_colorName("rouge");
    m_paletteColor->setName(test_colorName);
    QString result=m_paletteColor->getName();
    QVERIFY(result==test_colorName);
}
void PaletteModelTest::paletteColorConstructor()
{
    PaletteColor test_paletteColor(QColor(1,2,4),"texte",QPalette::Active,QPalette::Highlight);
    QVERIFY2(test_paletteColor.getColor()==QColor(1,2,4),"color is not equal");
}


void PaletteModelTest::cleanupTestCase()
{
    delete m_paletteModel;
    delete m_paletteColor;
}


QTEST_MAIN(PaletteModelTest);

#include "tst_paletteModel.moc"
