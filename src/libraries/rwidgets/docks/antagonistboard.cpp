#include "antagonistboard.h"
#include "ui_antagonistboard.h"

#include <QBuffer>
#include <QByteArray>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMenu>

#include "controller/antagonistboardcontroller.h"
#include "data/campaign.h"
#include "data/character.h"
#include "model/genericmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "worker/utilshelper.h"

namespace campaign
{
namespace
{
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
} // namespace
AntagonistBoard::AntagonistBoard(campaign::CampaignEditor* editor, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AntagonistBoard)
    , m_ctrl(new AntagonistBoardController(editor))
    , m_createTokenAct(new QAction(tr("Edit Token")))
    , m_cloneCharacterAct(new QAction(tr("Clone Character")))
    , m_changeImageAct(new QAction(tr("Change Image")))
{
    ui->setupUi(this);

    ui->m_antogonistView->setModel(m_ctrl->filteredModel());
    ui->m_antogonistView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->m_antogonistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_antogonistView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->m_antogonistView, &QTableView::customContextMenuRequested, this, &AntagonistBoard::contextualMenu);

    connect(ui->m_addCharacterAct, &QAction::triggered, this, [this]() { m_ctrl->addCharacter(); });
    connect(ui->m_removeCharacterAct, &QAction::triggered, this,
            [this]() { m_ctrl->removeCharacter(ui->m_antogonistView->currentIndex()); });

    ui->m_addCharacterBtn->setDefaultAction(ui->m_addCharacterAct);
    ui->m_removeCharacterBtn->setDefaultAction(ui->m_removeCharacterAct);

    ui->m_addPropertyBtn->setDefaultAction(ui->m_addPropertyAct);
    ui->m_addActionBtn->setDefaultAction(ui->m_addActionAct);
    ui->m_addShapeBtn->setDefaultAction(ui->m_addShapeAct);

    ui->m_removeActBtn->setDefaultAction(ui->m_removeActionAct);
    ui->m_removeShapeBtn->setDefaultAction(ui->m_removeShapeAct);
    ui->m_removePropertyBtn->setDefaultAction(ui->m_removePropertyAct);

    connect(ui->m_antogonistView, &QTableView::doubleClicked, this, [this](const QModelIndex& index) {
        if(!index.isValid() && index.column() != NonPlayableCharacterModel::ColTokenPath)
            return;
        auto uuid= index.data(NonPlayableCharacterModel::RoleUuid).toString();
        m_ctrl->editCharacter(uuid);
    });

    connect(m_changeImageAct.get(), &QAction::triggered, m_ctrl.get(), [this]() {
        auto filename= QFileDialog::getOpenFileName(this, tr("Select Image for character"), m_ctrl->imageFolder(),
                                                    helper::utils::allSupportedImageFormatFilter());

        if(!filename.isEmpty())
            m_ctrl->changeImage(m_currentItemId, filename);
    });

    connect(m_ctrl.get(), &AntagonistBoardController::characterChanged, this, [this]() {
        if(m_ctrl->editingCharacter())
        {
            auto character= m_ctrl->character();
            ui->m_nameEdit->setText(character->name());
            ui->m_colorSelector->setColor(character->getColor());
            ui->m_lifeMin->setValue(character->getHealthPointsMin());
            ui->m_maxLife->setValue(character->getHealthPointsMax());
            ui->m_currentLife->setValue(character->getHealthPointsCurrent());
            ui->m_initCommand->setText(character->getInitCommand());
            ui->m_initValue->setValue(character->getInitiativeScore());
            ui->m_avatarDisplay->setPixmap(QPixmap::fromImage(character->getAvatar()));
            ui->stackedWidget->setCurrentIndex(1);
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
        }
    });

    ui->m_actionList->setModel(m_ctrl->actionModel());
    ui->m_propertyList->setModel(m_ctrl->propertyModel());
    ui->m_shapeList->setModel(m_ctrl->shapeModel());

    connect(ui->m_addActionAct, &QAction::triggered, this,
            [this]() { m_ctrl->appendData(campaign::ModelType::Model_Action); });
    connect(ui->m_addPropertyAct, &QAction::triggered, this,
            [this]() { m_ctrl->appendData(campaign::ModelType::Model_Properties); });
    connect(ui->m_addActionAct, &QAction::triggered, this, [this]() {
        m_ctrl->appendData(campaign::ModelType::Model_Shape);
        setAllRowToHeight(m_ctrl->shapeModel(), ui->m_sizeEdit->value(), ui->m_shapeList);
    });

    ui->m_removeActionAct->setData(QVariant::fromValue(campaign::ModelType::Model_Action));
    ui->m_removePropertyAct->setData(QVariant::fromValue(campaign::ModelType::Model_Properties));
    ui->m_removeShapeAct->setData(QVariant::fromValue(campaign::ModelType::Model_Shape));

    auto remove= [this]() {
        auto act= qobject_cast<QAction*>(sender());
        auto mode= act->data().value<campaign::ModelType>();
        QModelIndex index;
        switch(mode)
        {
        case campaign::ModelType::Model_Action:
            index= ui->m_actionList->currentIndex();
            break;
        case campaign::ModelType::Model_Shape:
            index= ui->m_shapeList->currentIndex();
            break;
        case campaign::ModelType::Model_Properties:
            index= ui->m_propertyList->currentIndex();
            break;
        }
        m_ctrl->removeData(index, mode);
    };

    connect(ui->m_removeActionAct, &QAction::triggered, this, remove);
    connect(ui->m_removePropertyAct, &QAction::triggered, this, remove);
    connect(ui->m_removeShapeAct, &QAction::triggered, this, remove);

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
            // m_shapeModel->setData(index, file);
        }
    });

    connect(ui->m_sizeEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, [=] {
        auto val= ui->m_sizeEdit->value();
        ui->m_shapeList->setIconSize(QSize(val, val));
        // setAllRowToHeight(m_shapeModel, val, ui->m_shapeList);
    });

    connect(ui->m_avatarEdit, &QLineEdit::textChanged, this, &AntagonistBoard::updateImage);
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

AntagonistBoard::~AntagonistBoard()
{
    delete ui;
}

void AntagonistBoard::contextualMenu(const QPoint& pos)
{
    QMenu menu;

    auto index= ui->m_antogonistView->indexAt(pos);
    m_currentItemId= index.isValid() ? index.data(NonPlayableCharacterModel::RoleUuid).toString() : QString();

    menu.addAction(ui->m_addCharacterAct);
    menu.addAction(m_cloneCharacterAct.get());
    menu.addAction(ui->m_removeCharacterAct);
    menu.addAction(m_createTokenAct.get());
    menu.addAction(m_changeImageAct.get());

    m_createTokenAct->setEnabled(index.isValid());
    m_cloneCharacterAct->setEnabled(index.isValid());
    ui->m_removeCharacterAct->setEnabled(index.isValid());
    m_changeImageAct->setEnabled(index.isValid());

    menu.exec(ui->m_antogonistView->mapToGlobal(pos));
}

void AntagonistBoard::updateImage()
{
    QPixmap pix(ui->m_avatarEdit->text());
    ui->m_avatarDisplay->setPixmap(pix);
}
} // namespace campaign
