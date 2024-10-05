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
#include <QAbstractItemModelTester>
#include <QObject>
#include <QString>
#include <QtTest>

#include "controller/antagonistboardcontroller.h"
#include "model/nonplayablecharactermodel.h"
#include <memory>

struct PropertyInfo
{
    QString name;
    QVariantList values;
    QList<int> matchingCount;
};

void initModel(campaign::NonPlayableCharacterModel* model)
{
    // name, path, descr, color, distanceperturn, hp, initVal, lifeColor, minhp, maxhp, size, state Id, tags, id,
    // hasAct, hasProp, hasShape
    std::vector<std::tuple<QString, QString, QString, QColor, int, int, int, QColor, int, int, int, QString,
                           QStringList, QString, bool, bool, bool, QString, QString>>
        data{
            {"Lynn Gray-Rike",
             "",
             "blond with a gun",
             QColor(Qt::blue),
             0,
             100,
             0,
             QColor(Qt::green),
             0,
             100,
             64,
             "safe",
             {"cops", "girl", "swat", "law"},
             "lynn",
             true,
             true,
             true,
             "",
             ""},
            {"Otomo Akamu",
             "",
             "Ikoma's shadow",
             QColor(Qt::darkGreen),
             0,
             49,
             10,
             QColor(Qt::green),
             0,
             100,
             64,
             "injured",
             {"l5r", "secret", "honor", "spouse"},
             "akamu",
             false,
             false,
             false,
             "4G3",
             "gm details"},
            {"Ikoma Kae",
             "",
             "Proud with beautiful voice",
             QColor(Qt::darkYellow),
             0,
             100,
             0,
             QColor(Qt::green),
             0,
             100,
             64,
             "safe",
             {"l5r", "singer", "general", "lion"},
             "kae",
             true,
             false,
             false,
             "1d10+10",
             "daughter of the bad guy"},
            {"Toku Kahime",
             "",
             "The young prodigy",
             QColor(Qt::darkRed),
             0,
             12,
             35,
             QColor(Qt::green),
             0,
             100,
             64,
             "heavily injured",
             {"l5r", "champion", "monkey", "samurai"},
             "kahime",
             true,
             true,
             false,
             "",
             "Afraid of hawks"},
            {"John Northa",
             "",
             "Long sword actor",
             QColor(Qt::cyan),
             0,
             100,
             0,
             QColor(Qt::green),
             0,
             100,
             64,
             "safe",
             {"mv", "angel", "sword", "faith"},
             "john",
             true,
             false,
             true,
             "6d10",
             ""},
            {"Luna Gaztelumendi",
             "",
             "Demon with sniper rifle",
             QColor(Qt::darkMagenta),
             0,
             0,
             0,
             QColor(Qt::green),
             0,
             100,
             64,
             "dead",
             {"ins", "girl", "demon", "succube"},
             "luna",
             false,
             true,
             true,
             "",
             ""},
        };

    for(auto& character : data)
    {
        auto c= new campaign::NonPlayableCharacter();
        c->setName(std::get<0>(character));
        c->setAvatarPath(std::get<1>(character));
        c->setDescription(std::get<2>(character));
        c->setColor(std::get<3>(character));
        c->setDistancePerTurn(std::get<4>(character));
        c->setHealthPointsCurrent(std::get<5>(character));
        c->setInitiativeScore(std::get<6>(character));
        c->setLifeColor(std::get<7>(character));
        c->setHealthPointsMin(std::get<8>(character));
        c->setHealthPointsMax(std::get<9>(character));
        c->setSize(std::get<10>(character));
        c->setStateId(std::get<11>(character));
        c->setTags(std::get<12>(character));
        c->setUuid(std::get<13>(character));

        if(std::get<14>(character))
        { // actions
            c->addAction(new CharacterAction());
        }
        if(std::get<15>(character))
        { // properties
            c->addProperty(new CharacterProperty());
        }
        if(std::get<16>(character))
        { // shape
            c->addShape(new CharacterShape());
        }
        c->setInitCommand(std::get<17>(character));
        c->setGameMasterDesc(std::get<18>(character));

        model->addCharacter(c);
    }
}

class TestAntaFilter : public QObject
{
    Q_OBJECT
public:
    TestAntaFilter(QObject* parent= nullptr);

private slots:
    void init();
    void cleanup();

    void advancedFilterModel();
    void advancedFilterModel_data();

private:
    std::unique_ptr<campaign::NonPlayableCharacterModel> m_sourceModel;
    std::unique_ptr<campaign::FilteredCharacterModel> m_filterModel;
};

TestAntaFilter::TestAntaFilter(QObject* parent) : QObject(parent) {}

void TestAntaFilter::init()
{
    m_sourceModel.reset(new campaign::NonPlayableCharacterModel(nullptr));
    m_filterModel.reset(new campaign::FilteredCharacterModel());
    new QAbstractItemModelTester(m_sourceModel.get());

    initModel(m_sourceModel.get());
    m_filterModel->setSourceModel(m_sourceModel.get());

    new QAbstractItemModelTester(m_filterModel.get());
}

void TestAntaFilter::cleanup() {}

void TestAntaFilter::advancedFilterModel()
{
    QFETCH(QString, propertyName);
    QFETCH(QVariant, propertyValue);
    QFETCH(int, result);

    m_filterModel->setAdvanced(true);

    auto meta= m_filterModel->metaObject();
    auto property= meta->property(meta->indexOfProperty(propertyName.toStdString().c_str()));

    if(!property.write(m_filterModel.get(), propertyValue))
        qDebug() << "fail to write";

    QCOMPARE(m_filterModel->rowCount(), result);
}
void TestAntaFilter::advancedFilterModel_data()
{
    QTest::addColumn<QString>("propertyName");
    QTest::addColumn<QVariant>("propertyValue");
    QTest::addColumn<int>("result");

    static QList<PropertyInfo> infos{
        {"search", {"", "lynn", "otomo", "ikoma", "john", "kahime", "luna", "ka", "zzz"}, {6, 1, 1, 1, 1, 1, 1, 3, 0}},
        {"exclude",
         {"", "lynn", "otomo", "ikoma", "john", "kahime", "luna", "ka", "zzz", "a"},
         {6, 5, 5, 5, 5, 5, 5, 3, 6, 0}},
        {"tags", {"", "l5r", "singer", "girl"}, {6, 3, 1, 2}},
        {"excludeTags", {"", "l5r", "singer", "girl"}, {6, 3, 5, 4}},
        {"avatarDefinition",
         {campaign::FilteredCharacterModel::All, campaign::FilteredCharacterModel::With,
          campaign::FilteredCharacterModel::Without},
         {6, 0, 6}},
        {"initiativeScoreDef",
         {campaign::FilteredCharacterModel::All, campaign::FilteredCharacterModel::With,
          campaign::FilteredCharacterModel::Without},
         {6, 2, 4}},
        {"actionDef",
         {campaign::FilteredCharacterModel::All, campaign::FilteredCharacterModel::With,
          campaign::FilteredCharacterModel::Without},
         {6, 4, 2}},
        {"shapeDef",
         {campaign::FilteredCharacterModel::All, campaign::FilteredCharacterModel::With,
          campaign::FilteredCharacterModel::Without},
         {6, 3, 3}},
        {"propertiesDef",
         {campaign::FilteredCharacterModel::All, campaign::FilteredCharacterModel::With,
          campaign::FilteredCharacterModel::Without},
         {6, 3, 3}},
        {"initiativeCmdDef",
         {campaign::FilteredCharacterModel::All, campaign::FilteredCharacterModel::With,
          campaign::FilteredCharacterModel::Without},
         {6, 3, 3}},
        {"gmdetailsDef",
         {campaign::FilteredCharacterModel::All, campaign::FilteredCharacterModel::With,
          campaign::FilteredCharacterModel::Without},
         {6, 3, 3}},
        {"gmdetails", {"", "e", "hawk"}, {6, 2, 1}}};

    int i= 0;
    for(auto& info : infos)
    {
        if(info.matchingCount.size() != info.values.size())
        {
            qFatal("Bad test: values does not fit the matching count");
            continue;
        }
        int j= 0;
        for(const auto& value : info.values)
        {
            QTest::addRow("Filter [%d x %d]", i, j) << info.name << value << info.matchingCount[j];
            ++j;
        }
        ++i;
    }
}

QTEST_MAIN(TestAntaFilter);

#include "test_anta_filter.moc"
