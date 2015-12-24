/***************************************************************************
    *   Copyright (C) 2015 by Renaud Guezennec                                *
    *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
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
    ui->setupUi(this);
    uint seed = quintptr(this) + QDateTime::currentDateTime().toMSecsSinceEpoch();
    qsrand(seed);

    m_model << tr("First Name Japanese");
    ui->m_database->addItems(m_model);

    connect(ui->m_gen,SIGNAL(clicked()),this,SLOT(generateName()));

    DataBase jpMale;
    jpMale.filepath=":/data/result_Name_Japanese_Male.txt";
    jpMale.gender = Male;
    jpMale.id=0;



    m_gender.insert(jpMale.id,jpMale);
}

NameGeneratorWidget::~NameGeneratorWidget()
{
    delete ui;
}
void NameGeneratorWidget::generateName()
{
    ui->m_result->clear();
    DataBase base = m_gender.at(ui->m_database->currentIndex());

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

QString NameGeneratorWidget::buildName(const QJsonObject &json,int len)
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

    for(int index = 0; index< len; ++index)
    {
        bool lastIndex = (index==len-1) ? true : false;
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

        QJsonObject::iterator i;
        int sumDict=0;
        for(i = duoJson.begin(); i != duoJson.end(); ++i)
        {
            if(((i.key() != "0")    &&  (!lastIndex))||
                    ((lastIndex)       &&  (i.key() == "0")))
            {
                QJsonObject dictForTwoLetter = dataJson.find(duo.right(1)+i.key()).value().toObject();
                qDebug() << "First" << dictForTwoLetter.isEmpty() << duo.right(1)+i.key();

                if((!lastIndex)||((lastIndex)&&(dictForTwoLetter.find("0") != dictForTwoLetter.end() )))
                {
                    qDebug() << lastIndex << dictForTwoLetter<< index ;
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
                if(((i.key() != "0")&&(!lastIndex))||((lastIndex)&&(i.key() == "0")))
                {
                    QJsonObject dictForTwoLetter = dataJson.find(duo.right(1)+i.key()).value().toObject();
                    qDebug() << "Second" << dictForTwoLetter;
                    if((!lastIndex)||((lastIndex)&&(dictForTwoLetter.find("0") != dictForTwoLetter.end() )))
                    {
                        sumDict += i.value().toInt();
                        if(sumDict > value)
                        {
                            if(i.key() !="0")
                            {
                                result.append(i.key());
                            }
                            unfound=false;
                        }
                    }

                }
            }
        }

    }

    return result;
}
