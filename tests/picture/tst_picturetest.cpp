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
#include <memory>
#include "network/networkmessagereader.h"
#include "network/networkmessagewriter.h"

class PictureTest : public QObject
{
    Q_OBJECT

public:
    PictureTest();

private slots:
    void init();
    void cleanupTestCase();
    void testGetSet();
    void writeAndReadNetworkTest();
private:
    std::unique_ptr<Image> m_image;
};

PictureTest::PictureTest()
{
}

void PictureTest::init()
{
    m_image.reset(new Image());
}

void PictureTest::cleanupTestCase()
{
}

void PictureTest::testGetSet()
{
    CleverURI uri("girafe",":/assets/img/girafe.jpg",CleverURI::PICTURE);
    m_image->setCleverUri(&uri);
    QVERIFY2(*m_image->getCleverUri() == uri, "not the same image");

    QString ownerId("owner");
    m_image->setIdOwner(ownerId);
    QVERIFY2(m_image->isImageOwner(ownerId), "not the rigth owner");
}
void PictureTest::writeAndReadNetworkTest()
{
    for(int i = 0 ; i < 1000 ; ++i)
    {
        CleverURI uri("girafe",":/assets/img/girafe.jpg",CleverURI::PICTURE);
        m_image->setCleverUri(&uri);
        NetworkMessageWriter msg(NetMsg::MediaCategory,NetMsg::addMedia);

        m_image->fill(msg);

        auto const array = msg.getData();

        NetworkMessageReader msg2;
        msg2.setData(array);

        Image image2;
        image2.setCleverUriType(CleverURI::PICTURE);
        image2.readMessage(msg2);
        msg2.reset();
        msg.reset();

        //QCOMPARE( msg2.getSize() , msg.getSize() );

        QCOMPARE( m_image->getUriName(), image2.getUriName());
        QCOMPARE( m_image->getMediaId(), image2.getMediaId());
    }
}


QTEST_MAIN(PictureTest);

#include "tst_picturetest.moc"
