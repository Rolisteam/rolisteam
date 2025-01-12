#include <QTest>

#include "rwidgets/delegates/delegate.h"
#include <QAbstractItemModelTester>
#include <helper.h>
#include <limits>


class DelegateTest : public QObject
{
    Q_OBJECT
public:
    DelegateTest()= default;

private slots:
    void init();

private:
    std::unique_ptr<Delegate> m_delegate;
};

void DelegateTest::init()
{
    m_delegate.reset(new Delegate());
}


QTEST_MAIN(DelegateTest);
#include "tst_delegate.moc"
