#include "convertor.h"
#include "ui_convertor.h"



Convertor::Convertor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Convertor)
{
    ui->setupUi(this);
    connect(ui->m_categoryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(categoryHasChanged(int)));

    m_model = new UnitModel();

    Unit* meter = m_model->insertData(new Unit(QStringLiteral("meter"),QStringLiteral("m"),Unit::DISTANCE));
    Unit* km =m_model->insertData(new Unit(QStringLiteral("kilometer"),QStringLiteral("km"),Unit::DISTANCE));
    Unit* dam =m_model->insertData(new Unit(QStringLiteral("decameter"),QStringLiteral("dam"),Unit::DISTANCE));
    Unit* dm =m_model->insertData(new Unit(QStringLiteral("decimeter"),QStringLiteral("dm"),Unit::DISTANCE));
    Unit* hm =m_model->insertData(new Unit(QStringLiteral("hectometer"),QStringLiteral("hm"),Unit::DISTANCE));
    Unit* cm =m_model->insertData(new Unit(QStringLiteral("centimeter"),QStringLiteral("cm"),Unit::DISTANCE));
    Unit* mm =m_model->insertData(new Unit(QStringLiteral("millitimeter"),QStringLiteral("mm"),Unit::DISTANCE));
    Unit* po =m_model->insertData(new Unit(QStringLiteral("Inch"),QStringLiteral("po"),Unit::DISTANCE));
    Unit* ft =m_model->insertData(new Unit(QStringLiteral("Foot"),QStringLiteral("ft"),Unit::DISTANCE));
    Unit* yd = m_model->insertData(new Unit(QStringLiteral("Yard"),QStringLiteral("yd"),Unit::DISTANCE));
    Unit* mile =m_model->insertData(new Unit(QStringLiteral("Mile"),QStringLiteral("Mile"),Unit::DISTANCE));

    m_catModel = new CategoryModel(this);
    m_toModel = new CategoryModel(this);
    m_catModel->setSourceModel(m_model);
    m_toModel->setSourceModel(m_model);
    ui->m_fromCombo->setModel(m_catModel);
    ui->m_toCombo->setModel(m_toModel);



    //metter to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,km),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,dam),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,dm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,hm),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,cm),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,mm),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,po),new ConvertorOperator(39.3701));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,ft),new ConvertorOperator(3.28084));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,yd),new ConvertorOperator(1.09361));
    m_convertorTable.insert(QPair<Unit*,Unit*>(meter,mile),new ConvertorOperator(0.000621371));

    connect(ui->m_fromLine,SIGNAL(textChanged(QString)),this,SLOT(convert()));

}

Convertor::~Convertor()
{
    delete ui;
}

void Convertor::categoryHasChanged(int i)
{
    //ui->m_toCombo->clear();
    ui->m_toLine->clear();

    m_catModel->setCurrentCategory(m_model->getCatNameFromId((Unit::Category)i));
    m_toModel->setCurrentCategory(m_model->getCatNameFromId((Unit::Category)i));
}

void Convertor::convert()
{
    QModelIndex source = m_catModel->mapToSource(m_catModel->index(ui->m_fromCombo->currentIndex(),0));
    QModelIndex to = m_toModel->mapToSource(m_toModel->index(ui->m_toCombo->currentIndex(),0));


    Unit* from = m_model->getUnitFromIndex(source);
    Unit* dest= m_model->getUnitFromIndex(to);

    ConvertorOperator* convert = m_convertorTable.value(QPair<Unit*,Unit*>(from,dest));
    if(NULL!=convert)
    {
        double d = convert->convert(ui->m_fromLine->text().toDouble());
        ui->m_toLine->setText(QStringLiteral("%1").arg(d));
    }
}
