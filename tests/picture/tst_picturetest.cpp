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
#include <QtTest/QtTest>

#include <image.h>
#include <cleveruri.h>

class PictureTest : public QObject
{
    Q_OBJECT

public:
    PictureTest();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testGetSet();
private:
    Image* m_image;
};

PictureTest::PictureTest()
{
}

void PictureTest::initTestCase()
{
    m_image = new Image();
}

void PictureTest::cleanupTestCase()
{
    delete m_image;
}

void PictureTest::testGetSet()
{
    auto uri = new CleverURI("girafe",":/assets/img/girafe.jpg",CleverURI::PICTURE);
    m_image->setCleverUri(uri);
    QVERIFY2(m_image->getCleverUri() == uri, "not the same image");

    QString ownerId("owner");
    m_image->setIdOwner(ownerId);
    QVERIFY2(m_image->isImageOwner(ownerId), "not the rigth owner");
}


QTEST_MAIN(PictureTest);

#include "tst_picturetest.moc"
