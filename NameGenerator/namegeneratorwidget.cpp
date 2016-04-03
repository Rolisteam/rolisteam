/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://www.rolisteam.org/contact                   *
    *                                                                         *
    *   rolisteam is free software; you can redistribute it and/or modify     *
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

#include "namegeneratorwidget.h"
#include "ui_namegeneratorwidget.h"
#include <stdlib.h>
#include <QDateTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>


NameGeneratorWidget::NameGeneratorWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NameGeneratorWidget)
{
    setWindowTitle(QStringLiteral("Name Generator"));
    setObjectName(QStringLiteral("NameGenerator"));
    ui->setupUi(this);
    uint seed = quintptr(this) + QDateTime::currentDateTime().toMSecsSinceEpoch();
    qsrand(seed);

    m_model << tr("Japanese First Name")<< tr("Elve First Name")<< tr("Chinese First Name")<< tr("French First Name");
    ui->m_database->addItems(m_model);

    connect(ui->m_gen,SIGNAL(clicked()),this,SLOT(generateName()));

    DataBase jpMale;
    jpMale.filepath=":/data/result_Name_Japanese_Male.txt";
    jpMale.gender = Male;
    jpMale.id=0;

    DataBase jpFemale;
    jpFemale.filepath=":/data/result_Name_Japanese_Female.txt";
    jpFemale.gender = Female;
    jpFemale.id=1;

    DataBase elveMale;
    elveMale.filepath=":/data/result_Name_Elve_Male.txt";
    elveMale.gender = Male;
    elveMale.id=2;

    DataBase elveFemale ;
    elveFemale.filepath=":/data/result_Name_Elve_Female.txt";
    elveFemale.gender = Female;
    elveFemale.id=3;

    DataBase chineseMale;
    chineseMale.filepath=":/data/result_Name_Chinese_Male.txt";
    chineseMale.gender = Male;
    chineseMale.id=4;

    DataBase chineseFemale ;
    chineseFemale.filepath=":/data/result_Name_Chinese_Female.txt";
    chineseFemale.gender = Female;
    chineseFemale.id=5;

    DataBase frenchMale;
    frenchMale.filepath=":/data/result_Name_French_Male.txt";
    frenchMale.gender = Male;
    frenchMale.id=6;

    DataBase frenchFemale ;
    frenchFemale.filepath=":/data/result_Name_French_Female.txt";
    frenchFemale.gender = Female;
    frenchFemale.id=7;

    m_gender.insert(jpMale.id,jpMale);
    m_gender.insert(jpFemale.id,jpFemale);
    m_gender.insert(elveMale.id,elveMale);
    m_gender.insert(elveFemale.id,elveFemale);
    m_gender.insert(chineseMale.id,chineseMale);
    m_gender.insert(chineseFemale.id,chineseFemale);
    m_gender.insert(frenchMale.id,frenchMale);
    m_gender.insert(frenchFemale.id,frenchFemale);

}

NameGeneratorWidget::~NameGeneratorWidget()
{
    delete ui;
}
void NameGeneratorWidget::generateName()
{
    ui->m_result->clear();
    int i = ui->m_database->currentIndex()*2;
    if(ui->m_femaleBtn->isChecked())
    {
        ++i;
    }

    DataBase base = m_gender.at(i);
    qDebug()<< base.filepath;
    QFile loadFile(base.filepath);

    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
        return;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    for(int i = 0;i < ui->m_nameCount->value();++i)
    {
        QJsonObject json = loadDoc.object();
        QString str = buildName(json,ui->m_nameLenght->value());
        ui->m_result->append(str);
    }
}

QString NameGeneratorWidget::buildName(const QJsonObject& json,int len)
{
    QString result;
    QJsonObject firstLetter = json["firstLetter"].toObject();
    qreal first = (qreal)qrand()/(qreal)RAND_MAX;
    qreal sum=0;

    foreach (QString key, firstLetter.keys())
    {
        sum += firstLetter[key].toDouble();
        if((sum > first)&&(result.isEmpty()))
        {
            result = key;
        }
    }
    int rLen = len-1;//already find the first character.

    for(int index = 0; index< rLen; ++index)
    {
        bool lastIndex = result.size()+1==len ? true : false;
        bool beforeLast = ((result.size()+2)==len) ? true:false;
        QString duo;
        if(result.count()==1)
        {
            duo="0"+result;
        }
        else
        {
            duo=result.right(2);
        }

        QJsonObject dataJson = json["data"].toObject();
        QJsonObject duoJson = dataJson[duo.toLower()].toObject();
        QString lastestChar = result.right(1);

        if(lastIndex)
        {
            dataJson = json["lastLetter"].toObject();
            duoJson = dataJson[duo.toLower()].toObject();
        }


        QJsonObject::iterator i;
        int sumDict=0;
        for(i = duoJson.begin(); i != duoJson.end(); ++i)
        {
            if(i.key()!="0")
            {
                if(nextIsPossible(json,QStringLiteral("%1%2").arg(lastestChar).arg(i.key()),beforeLast))
                {
                    sumDict += i.value().toInt();
                }
            }
        }
        if(sumDict>0)
        {
            int value = qrand()%sumDict;
            sumDict=0;
            bool unfound=true;
            for(i = duoJson.begin(); i != duoJson.end() && unfound; ++i)
            {
                if(i.key()!="0")
                {

                    if(nextIsPossible(json,QStringLiteral("%1%2").arg(lastestChar).arg(i.key()),beforeLast))
                    {
                        sumDict += i.value().toInt();
                        if(sumDict > value)
                        {
                            result.append(i.key());
                            unfound=false;
                        }
                    }
                }
            }
        }

    }
    return result;
}
bool NameGeneratorWidget::nextCharacterCanEnd(const QJsonObject& json,QString key)
{
    QJsonObject dataJson = json["lastLetter"].toObject();
    if(dataJson[key.toLower()].isUndefined())
    {
        return false;
    }
    else
    {
        return true;
    }
}
bool NameGeneratorWidget::nextIsPossible(const QJsonObject& json,QString key,bool last)
{
    if(last)
    {
        return nextCharacterCanEnd(json,key);
    }
    QJsonObject dataJson = json["data"].toObject();
    if(dataJson[key.toLower()].isUndefined())
    {
        return false;
    }
    else
    {
        return true;
    }
}
