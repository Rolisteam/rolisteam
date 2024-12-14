#include <QAbstractItemModelTester>
#include <QObject>
#include <QtCore/QCoreApplication>
#include <QtTest/QtTest>

#include "helper.h"

#include "charactersheet/include/charactersheet/controllers/tablefield.h"
#include "controller/view_controller/charactersheetcontroller.h"
#include "rcse/fieldmodel.h"
#include "updater/media/charactersheetupdater.h"

class SheetTest : public QObject
{
    Q_OBJECT
public:
    SheetTest();

private slots:
    void init();

    void tableTest();
    void tableTest_data();

    void updateTest();

private:
    std::unique_ptr<FieldModel> m_model;
};

SheetTest::SheetTest() {}

void SheetTest::init()
{
    m_model.reset(new FieldModel);
    new QAbstractItemModelTester(m_model.get());
}

void SheetTest::tableTest()
{
    QFETCH(int, lineCount);
    QFETCH(int, colCount);
    QFETCH(int, expectedData);

    TableFieldController* field= new TableFieldController();
    m_model->appendField(field);

    for(auto i= 0; i < colCount; ++i)
        field->addColumn();

    for(auto i= 0; i < lineCount; ++i)
        field->addLine();

    QCOMPARE(m_model->rowCount(QModelIndex()), 1);
    QCOMPARE(m_model->rowCount(m_model->index(0, 0, QModelIndex())), expectedData);

    auto parent= m_model->index(0, 0, QModelIndex());
    auto count= m_model->rowCount(m_model->index(0, 0, QModelIndex()));
    for(int i= colCount + 1; i < count; ++i)
    {
        auto id= m_model->data(m_model->index(i, 0, parent), Qt::DisplayRole).toString();
        QString val= QString("cell_%1").arg(i - colCount);
        QCOMPARE(id, val);
    }
}

void SheetTest::tableTest_data()
{
    QTest::addColumn<int>("lineCount");
    QTest::addColumn<int>("colCount");
    QTest::addColumn<int>("expectedData");

    QTest::addRow("empty") << 0 << 0 << 1;
    QTest::addRow("empty") << 1 << 0 << 2;
    QTest::addRow("empty") << 3 << 1 << 8;
}

void SheetTest::updateTest()
{
    std::unique_ptr<CharacterSheetController> ctrl{new CharacterSheetController("mytestid")};
    std::unique_ptr<CharacterSheet> sheet{new CharacterSheet()};
    std::unique_ptr<CharacterSheetUpdater> updater{new CharacterSheetUpdater(nullptr, nullptr)};

    auto model= ctrl->model();
    model->addCharacterSheet(sheet.get());

    updater->addMediaController(ctrl.get());
}

QTEST_MAIN(SheetTest);

#include "tst_sheet.moc"
