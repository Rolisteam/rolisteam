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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <MainWindow.h>

// Contient tous les parametres extraits du fichier d'initialisation
Initialisation  G_initialisation;

// Identifiant du joueur en local sur la machine
QString G_idJoueurLocal;

ClientServeur * G_clientServeur;
MainWindow    * G_mainWindow;

class TestDice : public QObject
{
    Q_OBJECT

public:
    TestDice();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void testDiceRoll1();

    private:
    ChatWindow* m_chatwindow;
    PublicChat* m_chat;
};

TestDice::TestDice()
{
}

void TestDice::initTestCase()
{
    m_chat = new PublicChat();
    m_chatwindow = new ChatWindow(m_chat);
}

void TestDice::cleanupTestCase()
{
    delete m_chat;
    delete m_chatwindow;
}

void TestDice::testDiceRoll1()
{
    for(int i = 0; i< 2000; i++)
    {
       //  m_chatwindow->emettreTexte("!10");
    }

}

QTEST_MAIN(TestDice);

#include "tst_dice.moc"
