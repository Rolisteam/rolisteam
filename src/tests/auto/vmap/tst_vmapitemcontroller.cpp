#include <QtTest/QtTest>

#include "controller/item_controllers/characteritemcontroller.h"
#include "controller/view_controller/vectorialmapcontroller.h"

#include <helper.h>
#include <memory>

class CharacterItemCtrlTest : public QObject
{
    Q_OBJECT
public:
    CharacterItemCtrlTest();

private slots:
    void init();
    void propertiesTest();

private:
    std::unique_ptr<VectorialMapController> m_vmapCtrl;
    std::unique_ptr<vmap::CharacterItemController> m_ctrl;
};

CharacterItemCtrlTest::CharacterItemCtrlTest() {}
void CharacterItemCtrlTest::init()
{
    m_vmapCtrl.reset(new VectorialMapController("aaa"));

    m_ctrl.reset(new vmap::CharacterItemController({{Core::vmapkeys::KEY_CHARAC_NPC, false}}, m_vmapCtrl.get()));
}
void CharacterItemCtrlTest::propertiesTest()
{
    auto res= Helper::testAllProperties(m_ctrl.get(), {});
    for(const auto& f : res.second)
    {
        qDebug() << f << "unmanaged";
    }
}

QTEST_MAIN(CharacterItemCtrlTest)

#include "tst_vmapitemcontroller.moc"
