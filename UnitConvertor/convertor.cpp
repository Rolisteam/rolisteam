#include "convertor.h"
#include "ui_convertor.h"



Convertor::Convertor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Convertor)
{
    ui->setupUi(this);
    connect(ui->m_categoryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(categoryHasChanged(int)));

    m_model = new UnitModel();

    m_catModel = new CategoryModel(this);
    m_toModel = new CategoryModel(this);


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

    //kilometer to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,meter),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,dam),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,dm),new ConvertorOperator(10000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,hm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,cm),new ConvertorOperator(100000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,mm),new ConvertorOperator(1000000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,po),new ConvertorOperator(39370.1));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,ft),new ConvertorOperator(3280.84));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,yd),new ConvertorOperator(1093.61));
    m_convertorTable.insert(QPair<Unit*,Unit*>(km,mile),new ConvertorOperator(0.621371));

    //hectometer to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,meter),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,dam),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,dm),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,km),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,cm),new ConvertorOperator(10000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,mm),new ConvertorOperator(100000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,po),new ConvertorOperator(3937.01));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,ft),new ConvertorOperator(328.084));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,yd),new ConvertorOperator(109.361));
    m_convertorTable.insert(QPair<Unit*,Unit*>(hm,mile),new ConvertorOperator(0.0621371));

    //decameter to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,meter),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,hm),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,dm),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,km),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,cm),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,mm),new ConvertorOperator(10000));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,po),new ConvertorOperator(393.701));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,ft),new ConvertorOperator(32.8084));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,yd),new ConvertorOperator(10.9361));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dam,mile),new ConvertorOperator(0.00621371));

    //meter to *
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


    //decimeter to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,meter),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,dam),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,hm),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,km),new ConvertorOperator(0.0001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,cm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,mm),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,po),new ConvertorOperator(3.93701));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,ft),new ConvertorOperator(0.328084));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,yd),new ConvertorOperator(0.109361));
    m_convertorTable.insert(QPair<Unit*,Unit*>(dm,mile),new ConvertorOperator(0.00000621371));

    //centimeter to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,meter),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,hm),new ConvertorOperator(0.0001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,dm),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,km),new ConvertorOperator(0.000001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,dam),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,mm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,po),new ConvertorOperator(0.393701));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,ft),new ConvertorOperator(0.0328084));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,yd),new ConvertorOperator(0.0109361));
    m_convertorTable.insert(QPair<Unit*,Unit*>(cm,mile),new ConvertorOperator(0.000000621371));

    //milimeter to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,meter),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,hm),new ConvertorOperator(0.00001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,dm),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,km),new ConvertorOperator(0.0000001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,dam),new ConvertorOperator(0.0001));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,cm),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,po),new ConvertorOperator(0.0393701));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,ft),new ConvertorOperator(0.00328084));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,yd),new ConvertorOperator(0.00109361));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mm,mile),new ConvertorOperator(0.0000000621371));


    //mile to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,meter),new ConvertorOperator(1609.34));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,hm),new ConvertorOperator(16.0934));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,dm),new ConvertorOperator(1609340));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,km),new ConvertorOperator(1.60934));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,dam),new ConvertorOperator(160.934));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,cm),new ConvertorOperator(16093400));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,po),new ConvertorOperator(63360));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,ft),new ConvertorOperator(5280));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,yd),new ConvertorOperator(1760));
    m_convertorTable.insert(QPair<Unit*,Unit*>(mile,mm),new ConvertorOperator(1.609e+6));

    //yard to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,meter),new ConvertorOperator(0.9144));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,hm),new ConvertorOperator(0.009144));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,dm),new ConvertorOperator(9.144));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,km),new ConvertorOperator(0.0009144));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,dam),new ConvertorOperator(0.09144));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,cm),new ConvertorOperator(91.44));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,po),new ConvertorOperator(36));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,ft),new ConvertorOperator(3));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,mile),new ConvertorOperator(0.000568182));
    m_convertorTable.insert(QPair<Unit*,Unit*>(yd,mm),new ConvertorOperator(914.4));

    //ft to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,meter),new ConvertorOperator(0.3048));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,hm),new ConvertorOperator(0.003048));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,dm),new ConvertorOperator(3.048));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,km),new ConvertorOperator(0.0003048));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,dam),new ConvertorOperator(0.03048));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,cm),new ConvertorOperator(30.48));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,po),new ConvertorOperator(12));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,yd),new ConvertorOperator(0.333333));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,mile),new ConvertorOperator(0.000189394));
    m_convertorTable.insert(QPair<Unit*,Unit*>(ft,mm),new ConvertorOperator(304.8));


    //inch to *
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,meter),new ConvertorOperator(0.0254));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,hm),new ConvertorOperator(0.000254));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,dm),new ConvertorOperator(0.254));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,km),new ConvertorOperator(2.54e-5));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,dam),new ConvertorOperator(0.00254));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,cm),new ConvertorOperator(2.54));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,ft),new ConvertorOperator(0.0833333));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,yd),new ConvertorOperator(0.0277778));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,mile),new ConvertorOperator(1.5783e-5));
    m_convertorTable.insert(QPair<Unit*,Unit*>(po,mm),new ConvertorOperator(25.4));


    //////////////////////////
    // TEMPERATURE
    /////////////////////////

    Unit* celsius = m_model->insertData(new Unit(QStringLiteral("celsius"),QStringLiteral("°C"),Unit::TEMPERATURE));
    Unit* kelvin =m_model->insertData(new Unit(QStringLiteral("kelvin"),QStringLiteral("K"),Unit::TEMPERATURE));
    Unit* fahrenheit =m_model->insertData(new Unit(QStringLiteral("fahrenheit"),QStringLiteral("°F"),Unit::TEMPERATURE));



    m_convertorTable.insert(QPair<Unit*,Unit*>(celsius,kelvin),new ConvertorOperator(1,-273.15));
    m_convertorTable.insert(QPair<Unit*,Unit*>(celsius,fahrenheit),new ConvertorOperator(1.8,32));
    m_convertorTable.insert(QPair<Unit*,Unit*>(kelvin,celsius),new ConvertorOperator(1,273.15));
    m_convertorTable.insert(QPair<Unit*,Unit*>(fahrenheit,celsius),new ConvertorOperator(1.8,32,true));

    m_convertorTable.insert(QPair<Unit*,Unit*>(fahrenheit,kelvin),new ConvertorOperator(9.0/5.0,459.67,true));
    m_convertorTable.insert(QPair<Unit*,Unit*>(kelvin,fahrenheit),new ConvertorOperator(9.0/5.0,-459.67));


    m_catModel->setSourceModel(m_model);
    m_toModel->setSourceModel(m_model);
    ui->m_fromCombo->setModel(m_catModel);
    ui->m_toCombo->setModel(m_toModel);
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
