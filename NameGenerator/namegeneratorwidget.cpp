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

#define FName "FemaleName"
#define MName "MaleName"
#define LName "LastName"
#define BothName "BothName"

NameGeneratorWidget::NameGeneratorWidget(QWidget *parent) :
    GameMasterTool(parent),
    ui(new Ui::NameGeneratorWidget)
{
    setWindowTitle(QStringLiteral("Name Generator"));
    setObjectName(QStringLiteral("NameGenerator"));
    ui->setupUi(this);
    uint seed = quintptr(this) + QDateTime::currentDateTime().toMSecsSinceEpoch();
    qsrand(seed);

    m_model << tr("Chinese Name") << tr("Elve Name")<< tr("English Name") << tr("French Name") << tr("Japanese Name") << tr("Star Wars Name");
    ui->m_database->addItems(m_model);
    connect(ui->m_database,SIGNAL(currentIndexChanged(int)),this,SLOT(checkFeatureAvailability()));

    connect(ui->m_gen,SIGNAL(clicked()),this,SLOT(generateName()));

    //Japanese
    DataBase jpMale;
    jpMale.filepath=":/data/result_Name_Japanese_Male.txt";
    jpMale.gender = Male;
    jpMale.generate = true;
    jpMale.id=0;

    DataBase jpFemale;
    jpFemale.filepath=":/data/result_Name_Japanese_Female.txt";
    jpFemale.gender = Female;
    jpFemale.generate = true;
    jpFemale.id=1;


    //Elves
    DataBase elveMale;
    elveMale.filepath=":/data/result_Name_Elve_Male.txt";
    elveMale.gender = Male;
    elveMale.generate = true;
    elveMale.id=2;

    DataBase elveFemale ;
    elveFemale.filepath=":/data/result_Name_Elve_Female.txt";
    elveFemale.gender = Female;
    elveFemale.generate = true;
    elveFemale.id=3;


    //Chinese
    DataBase chineseMale;
    chineseMale.filepath=":/data/result_Name_Chinese_Male.txt";
    chineseMale.gender = Male;
    chineseMale.generate = true;
    chineseMale.id=4;

    DataBase chineseFemale ;
    chineseFemale.filepath=":/data/result_Name_Chinese_Female.txt";
    chineseFemale.gender = Female;
    chineseFemale.generate = true;
    chineseFemale.id=5;

    //French
    DataBase frenchMale;
    frenchMale.filepath=":/data/data_Name_French_Male.txt";
    frenchMale.gender = Male;
    frenchMale.generate = false;
    frenchMale.id=6;

    DataBase frenchFemale ;
    frenchFemale.filepath=":/data/data_Name_French_Female.txt";
    frenchFemale.gender = Female;
    frenchFemale.generate = false;
    frenchFemale.id=7;

    DataBase frenchName ;
    frenchName.filepath=":/data/data_Last_Name_French.txt";
    frenchName.gender = NONE;
    frenchName.generate = false;
    frenchName.id=8;

    //star wars
    DataBase swBoth;
    swBoth.filepath=":/data/result_Name_human_star_wars_Male.txt";
    swBoth.gender = BOTH;
    swBoth.generate = true;
    swBoth.id=9;

    DataBase swName;
    swName.filepath=":/data/result_Last_Name_human_star_wars_Male.txt";
    swName.gender = NONE;
    swName.generate = true;
    swName.id=10;

    //database
    QHash<QString,DataBase> chineseDataBase;
    chineseDataBase.insert(MName,chineseMale);
    chineseDataBase.insert(FName,chineseFemale);

    QHash<QString,DataBase> swDataBase;
    swDataBase.insert(BothName,swBoth);
    swDataBase.insert(LName,swName);

    QHash<QString,DataBase> elveDataBase;
    elveDataBase.insert(MName,elveMale);
    elveDataBase.insert(FName,elveFemale);

    QHash<QString,DataBase> jpnDataBase;
    jpnDataBase.insert(MName,jpMale);
    jpnDataBase.insert(FName,jpFemale);

    //create french database
    QHash<QString,DataBase> FrenchDataBase;
    FrenchDataBase.insert(MName,frenchMale);
    FrenchDataBase.insert(FName,frenchFemale);
    FrenchDataBase.insert(LName,frenchName);

    //inserting databases
    m_complexName.insert(Japanese,jpnDataBase);
    m_complexName.insert(Chinese,chineseDataBase);
    m_complexName.insert(Elves,elveDataBase);
    m_complexName.insert(French,FrenchDataBase);
    m_complexName.insert(StarWars,swDataBase);

    checkFeatureAvailability();
}

NameGeneratorWidget::~NameGeneratorWidget()
{
    delete ui;
}

void NameGeneratorWidget::buildAllNames(int count,QHash<QString,DataBase> data)
{
    QList<DataBase> list;
    if((ui->m_femaleBtn->isChecked())&&(ui->m_femaleBtn->isEnabled()))
    {
        list << data.value(FName);
    }
    else if((ui->m_maleBtn->isChecked())&&(ui->m_maleBtn->isEnabled()))
    {
        list << data.value(MName);
    }
    else
    {
        list << data.value(BothName);
    }
    if(ui->m_complexName->isChecked())
    {
        list << data.value(LName);
    }
    QList<QJsonDocument> generationData;
    QList<QStringList> pickUpData;

    for(auto database : list)
    {
        QFile loadFile(database.filepath);

        if (!loadFile.open(QIODevice::ReadOnly))
        {
            qWarning("Couldn't open save file.");
            return;
        }
        if(database.generate)
        {
            QByteArray saveData = loadFile.readAll();

            QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
            generationData.append(loadDoc);
        }
        else
        {
            QTextStream in(&loadFile);

            QStringList strData;
            while(!in.atEnd())
            {
                strData << in.readLine();
            }

            pickUpData.append(strData);
        }
    }
    int maxTry=30;
    for(int i = 0;i < count;++i)
    {
        int indexGenerated = 0;
        int indexPickUp = 0;
        QString result("");
        for(auto database : list)
        {
            QString tmpresult;

                if(database.generate)
                {
                    QJsonObject json = generationData.at(indexGenerated).object();
                    do
                    {
                        --maxTry;
                        tmpresult=buildName(json);
                    }
                    while(tmpresult.size()<2 && maxTry>0);
                    ++indexGenerated;
                }
                else
                {
                    do
                    {
                        --maxTry;
                        tmpresult = pickUpName(pickUpData.at(indexPickUp));
                    }
                    while(tmpresult.size()<2 && maxTry>0);
                    ++indexPickUp;
                }


            result+=QString("%1 ").arg(tmpresult);
        }
        ui->m_result->append(result);
    }

}

void NameGeneratorWidget::generateName()
{
    ui->m_result->clear();
    int i = ui->m_database->currentIndex()*2;
    if(ui->m_femaleBtn->isChecked())
    {
        ++i;
    }
    TypeOfGeneration index = (TypeOfGeneration)ui->m_database->currentIndex();

    QHash<QString,DataBase> data = m_complexName.value(index);

    buildAllNames(ui->m_nameCount->value(),data);
}
QString NameGeneratorWidget::pickUpName(QStringList data)
{
    if(data.isEmpty())
        return QString();
    int len =(qrand()%data.size());

    if(data.size()>len)
        return data.at(len);
    else
        return QString();

}

QString NameGeneratorWidget::buildName(const QJsonObject& json)
{
    int len =(qrand()%10)+2;
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
void NameGeneratorWidget::checkFeatureAvailability()
{
    TypeOfGeneration index = (TypeOfGeneration)ui->m_database->currentIndex();

    QHash<QString,DataBase> data = m_complexName.value(index);
    if(!data.contains(LName))
    {
        ui->m_complexName->setChecked(false);
        ui->m_complexName->setEnabled(false);
    }
    else
    {
        ui->m_complexName->setEnabled(true);
    }

    if(data.contains(FName))
    {
       ui->m_femaleBtn->setEnabled(true);
    }
    else
    {
       ui->m_femaleBtn->setEnabled(false);
       ui->m_femaleBtn->setChecked(false);
    }
    if(data.contains(MName))
    {
        ui->m_maleBtn->setEnabled(true);
    }
    else
    {
        ui->m_maleBtn->setEnabled(false);
        ui->m_maleBtn->setChecked(false);
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
