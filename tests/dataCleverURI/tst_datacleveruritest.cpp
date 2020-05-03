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

#include <QDir>
#include <QString>
#include <QtTest>
#include <data/cleveruri.h>

class DataCleverURITest : public QObject
{
    Q_OBJECT

public:
    DataCleverURITest();

private Q_SLOTS:
    void testCleverURISetGet();
    void testMode();
    void initTestCase();
    void cleanupTestCase();

private:
    CleverURI* m_cleverURI;
};

DataCleverURITest::DataCleverURITest() {}

void DataCleverURITest::initTestCase()
{
    m_cleverURI= new CleverURI(Core::ContentType::CHARACTERSHEET);
}

void DataCleverURITest::cleanupTestCase()
{
    delete m_cleverURI;
}
void DataCleverURITest::testCleverURISetGet()
{
    m_cleverURI->setContentType(Core::ContentType::CHARACTERSHEET);
    QVERIFY2(m_cleverURI->contentType() == Core::ContentType::CHARACTERSHEET, "CHARACTERSHEET is not the current type");

    m_cleverURI->setContentType(Core::ContentType::PICTURE);
    QVERIFY2(m_cleverURI->contentType() == Core::ContentType::PICTURE, "PICTURE is not the current type");

    /* m_cleverURI->setContentType(Core::ContentType::SONG);
     QVERIFY2(m_cleverURI->contentType() == Core::ContentType::SONG, "SONG is not the current type");

     m_cleverURI->setContentType(Core::ContentType::CHAT);
     QVERIFY2(m_cleverURI->contentType() == Core::ContentType::CHAT, "TCHAT is not the current type");*/

    m_cleverURI->setContentType(Core::ContentType::NOTES);
    QVERIFY2(m_cleverURI->contentType() == Core::ContentType::NOTES, "TEXT is not the current type");

    QString path("/foo/bar/file.map");
    m_cleverURI->setUri(path);
    m_cleverURI->setName(QStringLiteral("file"));
    QVERIFY2(m_cleverURI->getUri() == path, "URI is wrong!");

    QVERIFY2(m_cleverURI->getData(ResourcesNode::NAME).toString() == "file", "ShortName is wrong!");

    QVERIFY2(m_cleverURI->hasChildren() == false, "CleverURI has children, that should not be!");
}

void DataCleverURITest::testMode()
{
    /*m_cleverURI->setCurrentMode(CleverURI::Internal);
    QVERIFY2(m_cleverURI->getCurrentMode() == CleverURI::Internal, "Not the right mode INTERNAL");

    QString girafePath(":/img/girafe.jpg");
    m_cleverURI->setUri(girafePath);
    QVERIFY2(m_cleverURI->hasData(), "Clever URI has no data");

    m_cleverURI->clearData();
    QVERIFY2(!m_cleverURI->hasData(), "Clever URI has data");

    m_cleverURI->setCurrentMode(CleverURI::Linked);
    QVERIFY2(m_cleverURI->getCurrentMode() == CleverURI::Linked, "Not the right mode LINKED");

    QString lionPath(":/assets/img/lion.jpg");
    m_cleverURI->setUri(lionPath);
    QVERIFY2(!m_cleverURI->hasData(), "Clever URI has data");

    m_cleverURI->setCurrentMode(CleverURI::Internal);
    QVERIFY2(!m_cleverURI->hasData(), "Clever URI has not load the data");*/
}

QTEST_MAIN(DataCleverURITest);

#include "tst_datacleveruritest.moc"
