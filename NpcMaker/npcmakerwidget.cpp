#include "npcmakerwidget.h"
#include "ui_npcmakerwidget.h"

#include "data/character.h"
#include "genericmodel.h"
#include <QBuffer>
#include <QByteArray>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

//////////////////
/// Template Definition
//////////////////
/// Action
template <class T>
T* convertField(CharacterField* field)
{
    return dynamic_cast<T*>(field);
}

void setAllRowToHeight(GenericModel* model, int height, QTableView* view)
{
    for(int i= 0; i < model->rowCount(); ++i)
    {
        view->setRowHeight(i, height);
    }
}

NpcMakerWidget::NpcMakerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::NpcMakerWidget)
{
    ui->setupUi(this);

    ui->m_addPropertyBtn->setDefaultAction(ui->m_addPropertyAct);
    ui->m_addActionBtn->setDefaultAction(ui->m_addActionAct);
    ui->m_addShapeBtn->setDefaultAction(ui->m_addShapeAct);

    ui->m_removeActBtn->setDefaultAction(ui->m_removeActionAct);
    ui->m_removeShapeBtn->setDefaultAction(ui->m_removeShapeAct);
    ui->m_removePropertyBtn->setDefaultAction(ui->m_removePropertyAct);

    connect(ui->m_importNpc, &QPushButton::clicked, this, &NpcMakerWidget::importNpc);
    connect(ui->m_exportNpc, &QPushButton::clicked, this, &NpcMakerWidget::exportNpc);

    m_actionModel= new GenericModel({QStringLiteral("Name"), QStringLiteral("Comand")}, {}, this);
    ui->m_actionList->setModel(m_actionModel);
    connect(ui->m_addActionAct, &QAction::triggered, this, [=]() { m_actionModel->addData(new CharacterAction); });
    connect(ui->m_removeActionAct, &QAction::triggered, this, [=]() {
        auto index= ui->m_actionList->currentIndex();
        if(index.isValid())
            m_actionModel->removeData(index);
    });

    m_propertyModel= new GenericModel({QStringLiteral("Name"), QStringLiteral("Value")}, {}, this);
    ui->m_propertyList->setModel(m_propertyModel);
    connect(ui->m_addPropertyAct, &QAction::triggered, this,
            [=]() { m_propertyModel->addData(new CharacterProperty); });
    connect(ui->m_removePropertyAct, &QAction::triggered, this, [=]() {
        auto index= ui->m_propertyList->currentIndex();
        if(index.isValid())
            m_propertyModel->removeData(index);
    });

    m_shapeModel= new GenericModel({QStringLiteral("Name"), QStringLiteral("Uri")}, {1}, this);
    ui->m_shapeList->setModel(m_shapeModel);
    connect(ui->m_addShapeAct, &QAction::triggered, this, [=]() {
        m_shapeModel->addData(new CharacterShape);
        setAllRowToHeight(m_shapeModel, ui->m_sizeEdit->value(), ui->m_shapeList);
    });
    connect(ui->m_removeShapeAct, &QAction::triggered, this, [=]() {
        auto index= ui->m_shapeList->currentIndex();
        if(index.isValid())
            m_shapeModel->removeData(index);
    });
    connect(ui->m_shapeList, &QTableView::doubleClicked, this, [this](const QModelIndex& index) {
        auto path= index.data().toString();
        auto dir= QDir::homePath();

        if(!path.isEmpty())
        {
            QFileInfo info(path);
            dir= info.absolutePath();
        }

        auto file= QFileDialog::getOpenFileName(this, tr("Select Avatar"), dir,
                                                tr("Images (*.jpg *.jpeg *.png *.bmp *.svg)"));
        if(!file.isEmpty())
        {
            m_shapeModel->setData(index, file);
        }
    });

    connect(ui->m_sizeEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, [=] {
        auto val= ui->m_sizeEdit->value();
        ui->m_shapeList->setIconSize(QSize(val, val));
        setAllRowToHeight(m_shapeModel, val, ui->m_shapeList);
    });

    connect(ui->m_avatarEdit, &QLineEdit::textChanged, this, &NpcMakerWidget::updateImage);
    connect(ui->m_avatarOpenFileBtn, &QPushButton::clicked, this, [this]() {
        auto file= QFileDialog::getOpenFileName(this, tr("Select Avatar"), QDir::homePath(),
                                                tr("Images (*.jpg *.jpeg *.png *.bmp *.svg)"));
        if(!file.isEmpty())
        {
            ui->m_avatarEdit->setText(file);
            updateImage();
        }
    });
#ifndef Q_OS_MAC
    ui->m_actionList->setAlternatingRowColors(true);
    ui->m_shapeList->setAlternatingRowColors(true);
    ui->m_propertyList->setAlternatingRowColors(true);
#endif
}

NpcMakerWidget::~NpcMakerWidget()
{
    delete ui;
}
void NpcMakerWidget::importNpc()
{
    auto fileName= QFileDialog::getOpenFileName(this, tr("Import Token"), QDir::homePath(), tr("Token (*.rtok)"));

    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly))
    {
        QJsonDocument jdoc= QJsonDocument::fromJson(file.readAll());
        auto jobj= jdoc.object();

        ui->m_nameEdit->setText(jobj["name"].toString());
        ui->m_avatarEdit->setText(jobj["avatarUri"].toString());
        ui->m_initCommand->setText(jobj["initCommand"].toString());
        ui->m_initValue->setValue(jobj["initValue"].toInt());
        ui->m_sizeEdit->setValue(jobj["size"].toInt());
        ui->m_lifeMin->setValue(jobj["lifeMin"].toInt());
        ui->m_maxLife->setValue(jobj["lifeMax"].toInt());
        ui->m_currentLife->setValue(jobj["lifeCurrent"].toInt());

        auto avatarData= QByteArray::fromBase64(jobj["avatarImg"].toString().toUtf8());
        QPixmap pix= QPixmap::fromImage(QImage::fromData(avatarData));
        ui->m_avatarDisplay->setPixmap(pix);

        QColor color;
        color.setNamedColor(jobj["color"].toString());
        ui->m_colorSelector->setColor(color);

        auto actionArray= jobj["actions"].toArray();
        for(auto act : actionArray)
        {
            auto obj= act.toObject();
            auto action= new CharacterAction();
            action->setName(obj["name"].toString());
            action->setCommand(obj["command"].toString());
            m_actionModel->addData(action);
        }

        auto propertyArray= jobj["properties"].toArray();
        for(auto pro : propertyArray)
        {
            auto obj= pro.toObject();
            auto property= new CharacterProperty();
            property->setName(obj["name"].toString());
            property->setValue(obj["value"].toString());
            m_propertyModel->addData(property);
        }

        auto shapeArray= jobj["shapes"].toArray();
        for(auto sha : shapeArray)
        {
            auto obj= sha.toObject();
            auto shape= new CharacterShape();
            shape->setName(obj["name"].toString());
            shape->setUri(obj["uri"].toString());
            auto avatarData= QByteArray::fromBase64(obj["avatarImg"].toString().toUtf8());
            QImage img= QImage::fromData(avatarData);
            shape->setImage(img);
            m_shapeModel->addData(shape);
        }
    }
}
void NpcMakerWidget::exportNpc()
{
    auto fileName= QFileDialog::getSaveFileName(this, tr("Export Token"), QDir::homePath(), tr("Token (*.rtok)"));

    if(fileName.isEmpty())
        return;

    if(!fileName.endsWith(".rtok"))
        fileName.append(".rtok");

    QJsonDocument jdoc;
    QJsonObject jobj;

    jobj["name"]= ui->m_nameEdit->text();
    jobj["avatarUri"]= ui->m_avatarEdit->text();
    jobj["initCommand"]= ui->m_initCommand->text();
    jobj["initValue"]= ui->m_initValue->value();
    jobj["size"]= ui->m_sizeEdit->value();
    jobj["color"]= ui->m_colorSelector->color().name(QColor::HexArgb);
    jobj["lifeMax"]= ui->m_maxLife->value();
    jobj["lifeMin"]= ui->m_lifeMin->value();
    jobj["lifeCurrent"]= ui->m_currentLife->value();

    auto pix= ui->m_avatarDisplay->pixmap();
    QByteArray data;
    QBuffer buf(&data);
    pix->toImage().save(&buf, "png");
    jobj["avatarImg"]= QString::fromUtf8(data.toBase64());

    QJsonArray actionArray;
    for(auto field : *m_actionModel)
    {
        auto act= convertField<CharacterAction>(field);
        if(act == nullptr)
            continue;
        QJsonObject actJson;
        actJson["name"]= act->name();
        actJson["command"]= act->command();
        actionArray.append(actJson);
    }
    jobj["actions"]= actionArray;

    QJsonArray propertyArray;
    for(auto field : *m_propertyModel)
    {
        auto proprety= convertField<CharacterProperty>(field);
        if(proprety == nullptr)
            continue;
        QJsonObject actJson;
        actJson["name"]= proprety->name();
        actJson["value"]= proprety->value();
        propertyArray.append(actJson);
    }
    jobj["properties"]= propertyArray;

    QJsonArray shapeArray;
    for(auto field : *m_shapeModel)
    {
        auto shape= convertField<CharacterShape>(field);
        if(shape == nullptr)
            continue;
        QJsonObject actJson;
        actJson["name"]= shape->name();
        actJson["uri"]= shape->uri();
        auto pix= shape->image();
        QByteArray data;
        QBuffer buf(&data);
        pix.save(&buf, "png");
        actJson["dataImg"]= QString::fromUtf8(data.toBase64());
        shapeArray.append(actJson);
    }
    jobj["shapes"]= shapeArray;

    jdoc.setObject(jobj);

    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(jdoc.toJson());
    }
}
void NpcMakerWidget::updateImage()
{
    QPixmap pix(ui->m_avatarEdit->text());
    ui->m_avatarDisplay->setPixmap(pix);
}
