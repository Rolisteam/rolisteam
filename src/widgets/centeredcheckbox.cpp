#include <QHBoxLayout>


#include "centeredcheckbox.h"


CenteredCheckBox::CenteredCheckBox(QWidget* wid)
    : QWidget(wid)
{

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
    layout->setAlignment( Qt::AlignCenter );

    m_editorCheckBox = new QCheckBox();
    layout->addWidget(m_editorCheckBox);

    connect( m_editorCheckBox, SIGNAL(stateChanged(int)),
             this, SIGNAL(commitEditor()) );
}

bool CenteredCheckBox::isCheckedDelegate() const
{
    bool checked = false;
    switch(m_editorCheckBox->checkState())
    {
    case Qt::Unchecked:
        checked=false;
        break;
    case Qt::PartiallyChecked:
    case Qt::Checked:
        checked=true;
        break;
    }
    return checked;
}

void CenteredCheckBox::setCheckedDelegate(bool checked)
{
    if(checked)
    {
        m_editorCheckBox->setCheckState(Qt::Checked);
    }
    else
    {
        m_editorCheckBox->setCheckState(Qt::Unchecked);
    }
    update();
}
