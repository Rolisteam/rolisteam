#include "convertor.h"
#include "ui_convertor.h"



Convertor::Convertor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Convertor)
{
    ui->setupUi(this);
    connect(ui->m_categoryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(categoryHasChanged(int)));

    m_model = new UnitModel();

    m_model->insertData(new Unit(QStringLiteral("meter"),QStringLiteral("m"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("kilometer"),QStringLiteral("km"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("decameter"),QStringLiteral("dam"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("decimeter"),QStringLiteral("dm"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("hectometer"),QStringLiteral("hm"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("centimeter"),QStringLiteral("cm"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("millitimeter"),QStringLiteral("mm"),Unit::DISTANCE));




    m_model->insertData(new Unit(QStringLiteral("millitimeter"),QStringLiteral("mm"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("millitimeter"),QStringLiteral("mm"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("millitimeter"),QStringLiteral("mm"),Unit::DISTANCE));
    m_model->insertData(new Unit(QStringLiteral("millitimeter"),QStringLiteral("mm"),Unit::DISTANCE));

}

Convertor::~Convertor()
{
    delete ui;
}

void Convertor::categoryHasChanged(int i)
{
    ui->m_toCombo->clear();
    ui->m_toLine->clear();

    ui->m_fromCombo->setModel();
}
