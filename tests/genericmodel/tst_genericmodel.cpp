
#include "widgets/gmtoolbox/NpcMaker/genericmodel.h"
#include "data/character.h"
#include <memory>
#include <QtTest/QtTest>

class TestField : public CharacterField
{
public:
    TestField();
    Type getType();
    QVariant getData(int col, int role);
    virtual bool setData(int col, QVariant value, int role);
public:
    QVariant m_value;
};

TestField::TestField()
{

}

CharacterField::Type TestField::getType()
{
    return CharacterField::Shape;
}

QVariant TestField::getData(int col, int role)
{
    return m_value;
}

bool TestField::setData(int col, QVariant value, int role)
{
    m_value = value;
    return true;
}



class GenericModelTest : public QObject
{
    Q_OBJECT
public:
    GenericModelTest();

private slots:
    void init();
    void addTest();
    void removeTest();
    void columnNumberTest();

private:
    std::unique_ptr<GenericModel> m_model;
};


void GenericModelTest::init()
{
    QStringList cols;
    cols << "id" << "value";
    m_model.reset(new GenericModel(cols));
}

GenericModelTest::GenericModelTest()
{

}

void GenericModelTest::addTest()
{
    TestField field;
    field.setData(0, 1, 0);
    m_model->addData(&field);

    auto index = m_model->index(0,0);
    QCOMPARE(field.getData(0,0), m_model->data(index,0));
    QCOMPARE(m_model->rowCount(QModelIndex()), 1);
}

void GenericModelTest::removeTest()
{
    TestField field;
    field.setData(0, 1, 0);
    m_model->addData(&field);

    auto index2 = m_model->index(0,0);
    QCOMPARE(field.getData(0,0), m_model->data(index2,0));
    QCOMPARE(m_model->rowCount(QModelIndex()), 1);

    auto index = m_model->index(0,0);

    m_model->removeData(index);

    QCOMPARE(m_model->rowCount(), 0);
}

void GenericModelTest::columnNumberTest()
{
    QCOMPARE(m_model->columnCount(QModelIndex()), 2);
}


QTEST_MAIN(GenericModelTest);

#include "tst_genericmodel.moc"
