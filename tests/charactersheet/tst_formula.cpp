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

#include <QtCore/QCoreApplication>
#include <QtCore/QString>
#include <QtTest/QtTest>

#include "formula/formulamanager.h"

class TestFormula : public QObject
{
    Q_OBJECT

public:
    TestFormula();

private slots:
    void initTestCase();
    void getAndSetTest();
    void commandsTest();
    void wrongCommandsTest();
    void cleanupTestCase();

private:
    Formula::FormulaManager* m_formulaMan;
    QHash<QString, QString>* m_variable;
};

TestFormula::TestFormula() {}

void TestFormula::initTestCase()
{
    m_formulaMan= new Formula::FormulaManager();
    m_variable= new QHash<QString, QString>();

    m_variable->insert("Name", "John Doe");
    m_variable->insert("weight", "85kg");
    m_variable->insert("age", "18");
    m_variable->insert("size", "1.69");
    m_variable->insert("strenght", "4");
    m_variable->insert("intelligence", "5");
    m_variable->insert("speed", "59");
    m_variable->insert("ecole", "Akodo");
    m_variable->insert("arme", "Katana");
    m_variable->insert("XP", "0");
    m_variable->insert("max health", "82");
    m_variable->insert("mana", "10000");
    m_variable->insert("level", "-1");
    m_variable->insert("agility", "-2.5");
    m_variable->insert("manipulation", "10");
    m_variable->insert("mental health", "90%");

    m_formulaMan->setConstantHash(m_variable);
}

void TestFormula::getAndSetTest() {}

void TestFormula::commandsTest()
{
    QStringList commands;

    commands << "=4*4"
             << "=10+8"
             << "=10-7"
             << "=50/5"
             << "=25-52"
             << "=${speed}+10"
             << "=${size}*10"
             << "=${level}-100"
             << "=2+4/4"
             << "=${manipulation}/100"
             << "=${size}*${speed}"
             << "=${agility}-100"
             << "=${agility}*100"
             << "=min(${intelligence},${strenght})"
             << "=max(${intelligence},${strenght})"
             << "=abs(${size})"
             << "=18*1.69-10/-1"
             << "=avg(${age},20})"
             << "=(10+2)*3"
             << "=(10-4)*3"
             << "=(10-4)+6)*2"
             << "=(10+2)*(10-8)";

    QStringList results;
    results << "16"
            << "18"
            << "3"
            << "10"
            << "-27"
            << "69"
            << "16.9"
            << "-101"
            << "3"
            << "0.1"
            << "99.71"
            << "-102.5"
            << "-250"
            << "4"
            << "5"
            << "1"
            << "40.42"
            << "19"
            << "36"
            << "18"
            << "24"
            << "24";

    for(int i= 0; i < commands.size(); ++i)
    {
        QString cmd= commands.at(i);
        QString result= results.at(i);

        QVariant a= m_formulaMan->getValue(cmd);
        qDebug() << a << result << cmd;
        QVERIFY2(a.toDouble() == result.toDouble(), cmd.toStdString().c_str());
    }
}
void TestFormula::wrongCommandsTest()
{
    QStringList commands;

    foreach(QString cmd, commands)
    {
        QVariant a= m_formulaMan->getValue(cmd);

        // QVERIFY2(a==,cmd.toStdString().c_str());
    }
}
void TestFormula::cleanupTestCase()
{
    delete m_formulaMan;
    delete m_variable;
}

QTEST_MAIN(TestFormula);

#include "tst_formula.moc"
