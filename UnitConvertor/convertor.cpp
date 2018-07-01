#include "convertor.h"
#include "ui_convertor.h"
#include <QMetaType>

namespace GMTOOL
{


Convertor::Convertor(QWidget *parent) :
    GameMasterTool(parent),
    ui(new Ui::Convertor),
    m_customRulesModel(new CustomRuleModel)
{
    qRegisterMetaType<Unit>();
    //qRegisterMetaType<Unit*>();
    ui->setupUi(this);
    connect(ui->m_categoryCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(categoryHasChanged(int)));
    connect(ui->m_categoryCombo2,SIGNAL(currentIndexChanged(int)),this,SLOT(categoryHasChangedOnSecondPanel(int)));
    connect(ui->m_reverseAct,&QAction::triggered,this,[=](){
        auto from = ui->m_fromCombo->currentIndex();
        ui->m_fromCombo->setCurrentIndex(ui->m_toCombo->currentIndex());
        ui->m_toCombo->setCurrentIndex(from);
    });

    ui->m_addBtn->setDefaultAction(ui->m_addUnitAction);
    ui->m_removeBtn->setDefaultAction(ui->m_removeUnit);
    ui->m_reverse->setDefaultAction(ui->m_reverseAct);
    ui->m_customValue->setDefaultAction(ui->m_customRuleAct);
    ui->m_backBtn->setDefaultAction(ui->m_back);

    connect(ui->m_customRuleAct,&QAction::triggered,this,[=](){
        ui->stackedWidget->setCurrentIndex(1);
    });

    connect(ui->m_back,&QAction::triggered,this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    ui->stackedWidget->setCurrentIndex(0);


    connect(ui->m_addUnitAction,&QAction::triggered,m_customRulesModel,&CustomRuleModel::insertUnit);


    m_model = new UnitModel(this);

    m_catModel = new CategoryModel(this);
    m_toModel = new CategoryModel(this);
    auto editCatModel = new CategoryModel(this);


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
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,meter),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,dam),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,dm),new ConvertorOperator(10000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,hm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,cm),new ConvertorOperator(100000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,mm),new ConvertorOperator(1000000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,po),new ConvertorOperator(39370.1));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,ft),new ConvertorOperator(3280.84));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,yd),new ConvertorOperator(1093.61));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(km,mile),new ConvertorOperator(0.621371));

    //hectometer to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,meter),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,dam),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,dm),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,km),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,cm),new ConvertorOperator(10000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,mm),new ConvertorOperator(100000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,po),new ConvertorOperator(3937.01));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,ft),new ConvertorOperator(328.084));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,yd),new ConvertorOperator(109.361));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(hm,mile),new ConvertorOperator(0.0621371));

    //decameter to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,meter),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,hm),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,dm),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,km),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,cm),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,mm),new ConvertorOperator(10000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,po),new ConvertorOperator(393.701));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,ft),new ConvertorOperator(32.8084));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,yd),new ConvertorOperator(10.9361));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dam,mile),new ConvertorOperator(0.00621371));

    //meter to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,km),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,dam),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,dm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,hm),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,cm),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,mm),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,po),new ConvertorOperator(39.3701));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,ft),new ConvertorOperator(3.28084));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,yd),new ConvertorOperator(1.09361));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(meter,mile),new ConvertorOperator(0.000621371));


    //decimeter to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,meter),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,dam),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,hm),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,km),new ConvertorOperator(0.0001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,cm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,mm),new ConvertorOperator(100));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,po),new ConvertorOperator(3.93701));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,ft),new ConvertorOperator(0.328084));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,yd),new ConvertorOperator(0.109361));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(dm,mile),new ConvertorOperator(0.00000621371));

    //centimeter to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,meter),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,hm),new ConvertorOperator(0.0001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,dm),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,km),new ConvertorOperator(0.000001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,dam),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,mm),new ConvertorOperator(10));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,po),new ConvertorOperator(0.393701));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,ft),new ConvertorOperator(0.0328084));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,yd),new ConvertorOperator(0.0109361));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cm,mile),new ConvertorOperator(0.000000621371));

    //milimeter to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,meter),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,hm),new ConvertorOperator(0.00001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,dm),new ConvertorOperator(0.01));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,km),new ConvertorOperator(0.0000001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,dam),new ConvertorOperator(0.0001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,cm),new ConvertorOperator(0.1));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,po),new ConvertorOperator(0.0393701));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,ft),new ConvertorOperator(0.00328084));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,yd),new ConvertorOperator(0.00109361));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mm,mile),new ConvertorOperator(0.0000000621371));


    //mile to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,meter),new ConvertorOperator(1609.34));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,hm),new ConvertorOperator(16.0934));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,dm),new ConvertorOperator(1609340));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,km),new ConvertorOperator(1.60934));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,dam),new ConvertorOperator(160.934));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,cm),new ConvertorOperator(16093400));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,po),new ConvertorOperator(63360));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,ft),new ConvertorOperator(5280));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,yd),new ConvertorOperator(1760));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(mile,mm),new ConvertorOperator(1.609e+6));

    //yard to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,meter),new ConvertorOperator(0.9144));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,hm),new ConvertorOperator(0.009144));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,dm),new ConvertorOperator(9.144));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,km),new ConvertorOperator(0.0009144));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,dam),new ConvertorOperator(0.09144));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,cm),new ConvertorOperator(91.44));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,po),new ConvertorOperator(36));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,ft),new ConvertorOperator(3));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,mile),new ConvertorOperator(0.000568182));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(yd,mm),new ConvertorOperator(914.4));

    //ft to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,meter),new ConvertorOperator(0.3048));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,hm),new ConvertorOperator(0.003048));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,dm),new ConvertorOperator(3.048));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,km),new ConvertorOperator(0.0003048));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,dam),new ConvertorOperator(0.03048));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,cm),new ConvertorOperator(30.48));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,po),new ConvertorOperator(12));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,yd),new ConvertorOperator(0.333333));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,mile),new ConvertorOperator(0.000189394));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(ft,mm),new ConvertorOperator(304.8));


    //inch to *
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,meter),new ConvertorOperator(0.0254));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,hm),new ConvertorOperator(0.000254));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,dm),new ConvertorOperator(0.254));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,km),new ConvertorOperator(2.54e-5));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,dam),new ConvertorOperator(0.00254));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,cm),new ConvertorOperator(2.54));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,ft),new ConvertorOperator(0.0833333));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,yd),new ConvertorOperator(0.0277778));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,mile),new ConvertorOperator(1.5783e-5));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(po,mm),new ConvertorOperator(25.4));


    //////////////////////////
    // TEMPERATURE
    /////////////////////////

    Unit* celsius = m_model->insertData(new Unit(QStringLiteral("celsius"),QStringLiteral("°C"),Unit::TEMPERATURE));
    Unit* kelvin =m_model->insertData(new Unit(QStringLiteral("kelvin"),QStringLiteral("K"),Unit::TEMPERATURE));
    Unit* fahrenheit =m_model->insertData(new Unit(QStringLiteral("fahrenheit"),QStringLiteral("°F"),Unit::TEMPERATURE));



    m_convertorTable.insert(QPair<const Unit*, const Unit*>(celsius,kelvin),new ConvertorOperator(1,-273.15));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(celsius,fahrenheit),new ConvertorOperator(1.8,32));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kelvin,celsius),new ConvertorOperator(1,273.15));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(fahrenheit,celsius),new ConvertorOperator(1.8,-32,true));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(fahrenheit,kelvin),new ConvertorOperator(9.0/5.0,459.67,true));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kelvin,fahrenheit),new ConvertorOperator(9.0/5.0,-459.67));



    //////////////////////////
    // Currency
    /////////////////////////
    Unit* koku = m_model->insertData(new Unit(QStringLiteral("koku"),QStringLiteral("koku"),Unit::CURRENCY));
    Unit* bu =m_model->insertData(new Unit(QStringLiteral("bu"),QStringLiteral("bu"),Unit::CURRENCY));
    Unit* zeni =m_model->insertData(new Unit(QStringLiteral("zeni"),QStringLiteral("zeni"),Unit::CURRENCY));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(koku,bu),new ConvertorOperator(5));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(koku,zeni),new ConvertorOperator(50));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(bu,koku),new ConvertorOperator(0.2));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(bu,zeni),new ConvertorOperator(10));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(zeni,koku),new ConvertorOperator(0.02));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(zeni,bu),new ConvertorOperator(0.1));



    //////////////////////////
    // MASS
    /////////////////////////
    Unit* gram = m_model->insertData(new Unit(QStringLiteral("gram"),QStringLiteral("g"),Unit::MASS));
    Unit* kilo =m_model->insertData(new Unit(QStringLiteral("kilogram"),QStringLiteral("Kg"),Unit::MASS));
    Unit* once =m_model->insertData(new Unit(QStringLiteral("Once"),QStringLiteral("oz"),Unit::MASS));
    Unit* pound =m_model->insertData(new Unit(QStringLiteral("Pound"),QStringLiteral("lb"),Unit::MASS));


    m_convertorTable.insert(QPair<const Unit*, const Unit*>(gram,kilo),new ConvertorOperator(0.001));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(gram,once),new ConvertorOperator(1.0/28.349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(gram,pound),new ConvertorOperator(1.0/453.59237));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kilo,gram),new ConvertorOperator(1000));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kilo,once),new ConvertorOperator(1.0/0.028349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(kilo,pound),new ConvertorOperator(1.0/0.45359237));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(once,gram),new ConvertorOperator(28.349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(once,kilo),new ConvertorOperator(0.028349));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(once,pound),new ConvertorOperator(1.0/16.0));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(pound,gram),new ConvertorOperator(453.59237));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(pound,kilo),new ConvertorOperator(0.45359237));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(pound,once),new ConvertorOperator(16.0));


    ////////////////////////////
    // Volume
    ////////////////////////////
    Unit* cubicMeter = m_model->insertData(new Unit(QStringLiteral("Cubic Meter"),QStringLiteral("m³"),Unit::VOLUME));
    Unit* cubicFeet =m_model->insertData(new Unit(QStringLiteral("Cubic Feet"),QStringLiteral("ft³"),Unit::VOLUME));

    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cubicMeter,cubicFeet),new ConvertorOperator(35.3147));
    m_convertorTable.insert(QPair<const Unit*, const Unit*>(cubicFeet,cubicMeter),new ConvertorOperator(0.0283168));




    m_catModel->setSourceModel(m_model);
    m_toModel->setSourceModel(m_model);
    editCatModel->setSourceModel(m_model);

    m_customRulesModel->setUnits(editCatModel);
    m_customRulesModel->setConvertionRules(&m_convertorTable);

    ui->m_tableView->setModel(m_customRulesModel);

    ui->m_fromCombo->setModel(m_catModel);
    ui->m_toCombo->setModel(m_toModel);
    connect(ui->m_fromLine,SIGNAL(textChanged(QString)),this,SLOT(convert()));
    connect(ui->m_convert,SIGNAL(clicked(bool)),this,SLOT(convert()));
    connect(ui->m_fromCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(convert()));
    connect(ui->m_toCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(convert()));

    categoryHasChanged(0);
    categoryHasChangedOnSecondPanel(0);

}

Convertor::~Convertor()
{
    qDeleteAll(m_convertorTable);
    delete ui;
}

void Convertor::categoryHasChanged(int i)
{
    ui->m_toLine->clear();
    m_catModel->setCurrentCategory(m_model->getCatNameFromId((Unit::Category)i));
    m_toModel->setCurrentCategory(m_model->getCatNameFromId((Unit::Category)i));
}
void Convertor::categoryHasChangedOnSecondPanel(int i)
{
    m_customRulesModel->setCurrentCategory(m_model->getCatNameFromId(static_cast<Unit::Category>(i)),i);
}

void Convertor::convert()
{
    QModelIndex source = m_catModel->index(ui->m_fromCombo->currentIndex(),0);
    QModelIndex to = m_toModel->index(ui->m_toCombo->currentIndex(),0);

    if(!source.isValid() || !to.isValid())
        return;

    Unit* from = m_model->getUnitFromIndex(source,ui->m_categoryCombo->currentIndex());
    Unit* dest= m_model->getUnitFromIndex(to,ui->m_categoryCombo->currentIndex());

    ConvertorOperator* convert = m_convertorTable.value(QPair<const Unit*, const Unit*>(from,dest));
    if(nullptr!=convert)
    {
        double d = convert->convert(ui->m_fromLine->text().toDouble());
        ui->m_toLine->setText(QStringLiteral("%1").arg(d));
    }
}
}
