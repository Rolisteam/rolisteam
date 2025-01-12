#include <QTest>

#include "rwidgets/delegates/statedelegate.h"
#include "model/characterstatemodel.h"
#include <helper.h>
#include <QComboBox>


class StateDelegateTest : public QObject
{
    Q_OBJECT
public:
    StateDelegateTest() = default;

private slots:
    void init();

    void setAndGetTest();
    void nullableTest();

private:
    std::unique_ptr<StateDelegate> m_delegate;
    std::unique_ptr<CharacterStateModel> m_states;
    std::unique_ptr<QComboBox> m_combox;
};


void StateDelegateTest::init()
{
    m_combox.reset(new QComboBox);

    m_states.reset(new CharacterStateModel);
    CharacterState a;
    a.setId("ii");
    a.setLabel("State 1");
    a.setColor(Qt::green);
    a.setImagePath(":/img/arbre_square_500.jpg");
    m_states->appendState(std::move(a));

    CharacterState b;
    b.setId("iO");
    b.setLabel("State 2");
    b.setColor(Qt::blue);
    b.setImagePath(":/img/lion.jpg");
    m_states->appendState(std::move(b));

    m_delegate.reset(new StateDelegate(m_states.get()));
}

void StateDelegateTest::setAndGetTest()
{
    m_combox->setModel(m_states.get());
    m_combox->setItemDelegate(m_delegate.get());

    m_combox->show();

    m_combox->setCurrentIndex(1);

    m_combox->setCurrentIndex(2);

}

void StateDelegateTest::nullableTest()
{
    m_delegate->setEditorData(nullptr, QModelIndex());
    m_delegate->setModelData(nullptr, nullptr, QModelIndex());
}


QTEST_MAIN(StateDelegateTest);
#include "tst_state.moc"
