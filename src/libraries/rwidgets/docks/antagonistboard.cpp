#include "antagonistboard.h"
#include "ui_antagonistboard.h"

#include <QBuffer>
#include <QByteArray>
#include <QDrag>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMenu>

#include "controller/antagonistboardcontroller.h"
#include "controller/view_controller/imageselectorcontroller.h"
#include "data/campaign.h"
#include "data/campaigneditor.h"
#include "data/character.h"
#include "data/rolisteammimedata.h"
#include "model/genericmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "model/proxystatemodel.h"
#include "rwidgets/delegates/avatardelegate.h"
#include "rwidgets/delegates/statedelegate.h"
#include "rwidgets/delegates/taglistdelegate.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include "worker/iohelper.h"
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

campaign::FilteredCharacterModel::Definition checkDefinition(QCheckBox* with, QCheckBox* without, QCheckBox* sender)
{
    auto filter= campaign::FilteredCharacterModel::All;
    if(with->isChecked() && sender == with)
    {
        if(without->isChecked())
            without->setChecked(false);
        filter= campaign::FilteredCharacterModel::With;
    }
    else if(without->isChecked() && sender == without)
    {
        if(with->isChecked())
            with->setChecked(false);
        filter= campaign::FilteredCharacterModel::Without;
    }
    return filter;
}
} // namespace
AntagonistBoard::AntagonistBoard(campaign::CampaignEditor* editor, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AntagonistBoard)
    , m_ctrl(new AntagonistBoardController(editor))
    , m_createTokenAct(new QAction(tr("Edit Token")))
    , m_cloneCharacterAct(new QAction(tr("Clone Character")))
    , m_changeImageAct(new QAction(tr("Change Image")))
    , m_fullModeAct(new QAction(tr("Main columns")))
    , m_minimalModeAct(new QAction(tr("Minimal Columns")))
    , m_saveTokenAct(new QAction(tr("Save")))
{
    ui->setupUi(this);

    ui->m_antogonistView->setModel(m_ctrl->filteredModel());
    ui->m_antogonistView->setDragEnabled(true);
    ui->m_antogonistView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->m_antogonistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_antogonistView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->m_antogonistView->setItemDelegateForColumn(NonPlayableCharacterModel::ColAvatar, new AvatarDelegate(this));
    ui->m_antogonistView->setItemDelegateForColumn(NonPlayableCharacterModel::ColTags, new TagListDelegate(this));
    ui->m_antogonistView->setItemDelegateForColumn(NonPlayableCharacterModel::ColState,
                                                   new StateDelegate(m_ctrl->characterStateModel(), this));
    connect(ui->m_antogonistView, &QTableView::customContextMenuRequested, this, &AntagonistBoard::contextualMenu);
    m_fullModeAct->setCheckable(true);
    m_minimalModeAct->setCheckable(true);

    ui->m_currentState->setModel(new ProxyStateModel(m_ctrl->stateModel(), this));
    connect(ui->m_currentState, &QComboBox::currentIndexChanged, this,
            [this]()
            {
                auto idx= ui->m_currentState->currentIndex();
                auto model= m_ctrl->filteredModel();
                auto stateModel= ui->m_currentState->model();
                model->setCharacterStateId(
                    stateModel->data(stateModel->index(idx, 0), CharacterStateModel::ID).toString());
            });

    ui->m_antogonistView->setIconSize(QSize(64, 64));

    ui->m_saveBtn->setDefaultAction(m_saveTokenAct.get());
    ui->m_advencedBtn->setDefaultAction(ui->m_advencedAct);
    connect(ui->m_advencedAct, &QAction::triggered, this,
            [this](bool isChecked)
            {
                ui->m_advencedGroup->setVisible(isChecked);
                auto model= m_ctrl->filteredModel();
                model->setAdvanced(isChecked);
            });

    ui->m_advencedGroup->setVisible(false);

    connect(m_saveTokenAct.get(), &QAction::triggered, this,
            [this]
            {
                auto character= m_ctrl->character();

                character->setName(ui->m_nameEdit->text());
                character->setSize(ui->m_sizeEdit->value());
                character->setInitCommand(ui->m_initCommand->text());
                character->setInitiativeScore(ui->m_initValue->value());
                character->setColor(ui->m_colorSelector->color());
                character->setHealthPointsCurrent(ui->m_currentLife->value());
                character->setHealthPointsMax(ui->m_maxLife->value());
                character->setHealthPointsMin(ui->m_lifeMin->value());
                character->setAvatar(IOHelper::pixmapToData(ui->m_avatarDisplay->pixmap()));

                m_ctrl->saveToken();
                m_ctrl->setCharacter(nullptr);
            });

    ui->m_antogonistView->installEventFilter(this);

    connect(m_fullModeAct.get(), &QAction::toggled, this,
            [this](bool val)
            {
                if(!val)
                    return;

                m_minimalModeAct->setChecked(false);
                int i= 0;
                for(auto const& col : m_columnsAction)
                {
                    if(i < 4)
                        col->setChecked(true);
                    else
                        col->setChecked(false);

                    ++i;
                }
            });

    connect(m_minimalModeAct.get(), &QAction::toggled, this,
            [this](bool val)
            {
                if(!val)
                    return;
                int i= 0;
                m_fullModeAct->setChecked(false);
                for(auto const& col : m_columnsAction)
                {
                    if(i < 2)
                        col->setChecked(true);
                    else
                        col->setChecked(false);

                    ++i;
                }
            });

    if(!editor)
        return;

    auto cols= editor->campaign()->npcModel()->headers();

    int i= 0;
    std::for_each(std::begin(cols), std::end(cols),
                  [&i, this](const QString& colname)
                  {
                      std::unique_ptr<QAction> act(new QAction(colname));
                      act->setCheckable(true);
                      act->setChecked(true);
                      connect(act.get(), &QAction::toggled, this, &AntagonistBoard::hideColumn);
                      act->setData(i);
                      ++i;
                      m_columnsAction.push_back(std::move(act));
                  });

    connect(ui->m_addCharacterAct, &QAction::triggered, this, [this]() { m_ctrl->addCharacter(); });
    connect(ui->m_removeCharacterAct, &QAction::triggered, this,
            [this]()
            {
                auto index= ui->m_antogonistView->currentIndex();
                if(!index.isValid())
                    return;

                m_ctrl->removeCharacter(index.data(NonPlayableCharacterModel::RoleUuid).toString());
            });
    m_fullModeAct->setChecked(false);
    m_minimalModeAct->setChecked(true);

    ui->m_addCharacterBtn->setDefaultAction(ui->m_addCharacterAct);
    ui->m_removeCharacterBtn->setDefaultAction(ui->m_removeCharacterAct);

    ui->m_addPropertyBtn->setDefaultAction(ui->m_addPropertyAct);
    ui->m_addActionBtn->setDefaultAction(ui->m_addActionAct);
    ui->m_addShapeBtn->setDefaultAction(ui->m_addShapeAct);

    ui->m_removeActBtn->setDefaultAction(ui->m_removeActionAct);
    ui->m_removeShapeBtn->setDefaultAction(ui->m_removeShapeAct);
    ui->m_removePropertyBtn->setDefaultAction(ui->m_removePropertyAct);

    connect(ui->m_antogonistView, &DragableTableView::dragItem, this,
            [this](QModelIndex index)
            {
                if(index.column() != NonPlayableCharacterModel::ColAvatar)
                    return;
                auto uuid= index.data(NonPlayableCharacterModel::RoleUuid).toString();
                auto name= index.data(NonPlayableCharacterModel::RoleName).toString();
                auto img= index.data(NonPlayableCharacterModel::RoleAvatar);
                auto pix = img.value<QPixmap>();

                if(pix.isNull() || img.isNull())
                    return;
                QDrag* drag= new QDrag(this);
                RolisteamMimeData* mimeData= new RolisteamMimeData();

                mimeData->setNpcUuid(uuid);
                mimeData->setImageData(img);
                mimeData->setText(name);
                drag->setMimeData(mimeData);
                drag->setPixmap(helper::utils::roundCornerImage(pix));
                drag->exec();
            });

    connect(m_createTokenAct.get(), &QAction::triggered, this,
            [this]()
            {
                if(m_currentItemId.isEmpty())
                    return;
                m_ctrl->editCharacter(m_currentItemId);
            });

    auto changeImageFunc = [this](const QString& itemId)
    {
        ImageSelectorController ctrl(false, ImageSelectorController::All, ImageSelectorController::Square);
        ImageSelectorDialog dialog(&ctrl, this);
        if(QDialog::Accepted != dialog.exec())
            return;

        m_ctrl->changeImage(itemId, ctrl.finalImageData());
    };

    connect(m_changeImageAct.get(), &QAction::triggered, m_ctrl.get(),
            [this, changeImageFunc]()
            {
                changeImageFunc(m_currentItemId);
            });

    connect(m_ctrl.get(), &AntagonistBoardController::characterChanged, this,
            [this]()
            {
                if(m_ctrl->editingCharacter())
                {
                    auto character= m_ctrl->character();
                    ui->m_nameEdit->setText(character->name());
                    ui->m_colorSelector->setColor(character->getColor());
                    ui->m_lifeMin->setValue(character->getHealthPointsMin());
                    ui->m_maxLife->setValue(character->getHealthPointsMax());
                    ui->m_currentLife->setValue(character->getHealthPointsCurrent());
                    ui->m_initCommand->setText(character->initCommand());
                    ui->m_initValue->setValue(character->getInitiativeScore());
                    ui->m_avatarDisplay->setPixmap(IOHelper::dataToPixmap(character->avatar()));
                    ui->stackedWidget->setCurrentIndex(1);
                }
                else
                {
                    ui->stackedWidget->setCurrentIndex(0);
                }
            });
    ui->stackedWidget->setCurrentIndex(0);

    ui->m_actionList->setModel(m_ctrl->actionModel());
    ui->m_propertyList->setModel(m_ctrl->propertyModel());
    ui->m_shapeList->setModel(m_ctrl->shapeModel());

    connect(ui->m_addActionAct, &QAction::triggered, this,
            [this]() { m_ctrl->appendData(campaign::ModelType::Model_Action); });
    connect(ui->m_addPropertyAct, &QAction::triggered, this,
            [this]() { m_ctrl->appendData(campaign::ModelType::Model_Properties); });
    connect(ui->m_addActionAct, &QAction::triggered, this,
            [this]()
            {
                m_ctrl->appendData(campaign::ModelType::Model_Shape);
                setAllRowToHeight(m_ctrl->shapeModel(), ui->m_sizeEdit->value(), ui->m_shapeList);
            });

    ui->m_removeActionAct->setData(QVariant::fromValue(campaign::ModelType::Model_Action));
    ui->m_removePropertyAct->setData(QVariant::fromValue(campaign::ModelType::Model_Properties));
    ui->m_removeShapeAct->setData(QVariant::fromValue(campaign::ModelType::Model_Shape));

    auto remove= [this]()
    {
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

    connect(ui->m_antogonistView, &QTableView::doubleClicked, this, [changeImageFunc](const QModelIndex& index){
        if(!index.isValid())
            return;

        auto itemId = index.data(NonPlayableCharacterModel::RoleUuid).toString();

        if(itemId.isEmpty())
            return;

        switch(index.column())
        {
        case NonPlayableCharacterModel::ColAvatar:
            changeImageFunc(itemId);
            break;
        default:
            break;
        }


    });
    connect(ui->m_removeActionAct, &QAction::triggered, this, remove);
    connect(ui->m_removePropertyAct, &QAction::triggered, this, remove);
    connect(ui->m_removeShapeAct, &QAction::triggered, this, remove);

    connect(ui->m_shapeList, &QTableView::doubleClicked, this,
            [this](const QModelIndex& index)
            {
                auto path= index.data().toString();
                auto dir= QDir::homePath();

                if(!path.isEmpty())
                {
                    QFileInfo info(path);
                    dir= info.absolutePath();
                }

                ImageSelectorController ctrl(false, ImageSelectorController::All, ImageSelectorController::Square);
                ImageSelectorDialog dialog(&ctrl, this);
                if(QDialog::Accepted != dialog.exec())
                    return;

                auto data= ctrl.finalImageData();
                // m_shapeModel->setData(index, data);
            });

    connect(ui->m_sizeEdit, QOverload<int>::of(&QSpinBox::valueChanged), this,
            [=]
            {
                auto val= ui->m_sizeEdit->value();
                ui->m_shapeList->setIconSize(QSize(val, val));
                // setAllRowToHeight(m_shapeModel, val, ui->m_shapeList);
            });

    // sconnect(ui->m_avatarEdit, &QLineEdit::textChanged, this, &AntagonistBoard::updateImage);
    connect(ui->m_avatarOpenFileBtn, &QPushButton::clicked, this,
            [this]()
            {
                ImageSelectorController ctrl(true, ImageSelectorController::All, ImageSelectorController::Square);
                ImageSelectorDialog dialog(&ctrl, this);
                if(QDialog::Accepted != dialog.exec())
                    return;

                auto data= ctrl.finalImageData();
                updateImage(data);
            });

    connect(ui->m_tags, &QLineEdit::textEdited, this,
            [this]()
            {
                auto model= m_ctrl->filteredModel();
                if(model)
                    model->setTags(ui->m_tags->text());
            });

    auto avatarUpdate= [this]()
    {
        auto filtered= m_ctrl->filteredModel();
        if(filtered)
            filtered->setAvatarDefinition(
                checkDefinition(ui->m_withAvatar, ui->m_withoutAvatar, qobject_cast<QCheckBox*>(sender())));
    };

    connect(ui->m_withAvatar, &QCheckBox::clicked, this, avatarUpdate);
    connect(ui->m_withoutAvatar, &QCheckBox::clicked, this, avatarUpdate);

    auto initScoreUpdate= [this]()
    {
        auto filtered= m_ctrl->filteredModel();
        if(filtered)
            filtered->setInitiativeScoreDef(
                checkDefinition(ui->m_withInit, ui->m_withoutInit, qobject_cast<QCheckBox*>(sender())));
    };

    connect(ui->m_withInit, &QCheckBox::clicked, this, initScoreUpdate);
    connect(ui->m_withoutInit, &QCheckBox::clicked, this, initScoreUpdate);

    auto actionUpdate= [this]()
    {
        auto filtered= m_ctrl->filteredModel();
        if(filtered)
            filtered->setActionDef(
                checkDefinition(ui->m_withActions, ui->m_withoutAction, qobject_cast<QCheckBox*>(sender())));
    };

    connect(ui->m_withActions, &QCheckBox::clicked, this, actionUpdate);
    connect(ui->m_withoutAction, &QCheckBox::clicked, this, actionUpdate);

    auto propertiesUpdate= [this]()
    {
        auto filtered= m_ctrl->filteredModel();
        if(filtered)
            filtered->setPropertiesDef(
                checkDefinition(ui->m_withProperties, ui->m_withoutProperties, qobject_cast<QCheckBox*>(sender())));
    };

    connect(ui->m_withProperties, &QCheckBox::clicked, this, propertiesUpdate);
    connect(ui->m_withoutProperties, &QCheckBox::clicked, this, propertiesUpdate);

    auto shapeUpdate= [this]()
    {
        auto filtered= m_ctrl->filteredModel();
        if(filtered)
            filtered->setShapeDef(
                checkDefinition(ui->m_withShape, ui->m_withoutShape, qobject_cast<QCheckBox*>(sender())));
    };

    connect(ui->m_withShape, &QCheckBox::clicked, this, shapeUpdate);
    connect(ui->m_withoutShape, &QCheckBox::clicked, this, shapeUpdate);

    auto initiativeCmdUpdate= [this]()
    {
        auto filtered= m_ctrl->filteredModel();
        if(filtered)
            filtered->setInitiativeCmdDef(
                checkDefinition(ui->m_withInitCmd, ui->m_withoutInitCmd, qobject_cast<QCheckBox*>(sender())));
    };

    connect(ui->m_withInitCmd, &QCheckBox::clicked, this, initiativeCmdUpdate);
    connect(ui->m_withoutInitCmd, &QCheckBox::clicked, this, initiativeCmdUpdate);

    connect(ui->m_textToExclude, &QLineEdit::textEdited, this,
            [this](const QString& text)
            {
                auto model= m_ctrl->filteredModel();
                if(model)
                    model->setExclude(text);
            });

    connect(ui->m_exludedTags, &QLineEdit::textEdited, this,
            [this](const QString& text)
            {
                auto model= m_ctrl->filteredModel();
                if(model)
                    model->setExcludeTags(text);
            });
    auto updateHealth= [this]()
    {
        QCheckBox* sdr= qobject_cast<QCheckBox*>(sender());
        auto hlState= FilteredCharacterModel::HealthState::HS_All;

        if(sdr == ui->m_healthMin && ui->m_healthMin->isChecked())
        {
            hlState= FilteredCharacterModel::HealthState::HS_Dead;
            ui->m_injured->setChecked(false);
            ui->m_fullHealth->setChecked(false);
        }
        else if(sdr == ui->m_injured && ui->m_injured->isChecked())
        {
            hlState= FilteredCharacterModel::HealthState::HS_Injured;
            ui->m_healthMin->setChecked(false);
            ui->m_fullHealth->setChecked(false);
        }
        else if(sdr == ui->m_fullHealth && ui->m_fullHealth->isChecked())
        {
            hlState= FilteredCharacterModel::HealthState::HS_Full;
            ui->m_healthMin->setChecked(false);
            ui->m_injured->setChecked(false);
        }
        auto filtered= m_ctrl->filteredModel();
        if(filtered)
            filtered->setHlState(hlState);
    };
    connect(ui->m_healthMin, &QCheckBox::clicked, this, updateHealth);
    connect(ui->m_injured, &QCheckBox::clicked, this, updateHealth);
    connect(ui->m_fullHealth, &QCheckBox::clicked, this, updateHealth);

    ui->m_colorCombo->setModel(m_ctrl->colorModel());
    ui->m_currentState->setModel(m_ctrl->stateModel());

    connect(ui->m_searchLine, &QLineEdit::textChanged, this,
            [this]() { m_ctrl->setSearchText(ui->m_searchLine->text()); });

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

const std::vector<std::unique_ptr<QAction>>& AntagonistBoard::columnsActions() const
{
    return m_columnsAction;
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
    menu.addSeparator();
    menu.addAction(m_fullModeAct.get());
    menu.addAction(m_minimalModeAct.get());
    auto colsMenu= menu.addMenu(tr("Show Columns"));
    for(auto const& act : m_columnsAction)
    {
        colsMenu->addAction(act.get());
    }

    m_createTokenAct->setEnabled(index.isValid());
    m_cloneCharacterAct->setEnabled(index.isValid());
    ui->m_removeCharacterAct->setEnabled(index.isValid());
    m_changeImageAct->setEnabled(index.isValid());

    menu.exec(ui->m_antogonistView->mapToGlobal(pos));
}

void AntagonistBoard::updateImage(const QByteArray& data)
{
    QPixmap pix;
    pix.loadFromData(data);
    ui->m_avatarDisplay->setPixmap(pix);
}

void AntagonistBoard::hideColumn()
{
    auto act= qobject_cast<QAction*>(sender());
    auto i= act->data().toInt();

    if(act->isChecked())
        ui->m_antogonistView->showColumn(i);
    else
        ui->m_antogonistView->hideColumn(i);
}

bool AntagonistBoard::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == ui->m_antogonistView)
    {
        /*if(event->type() == QEvent::MouseMove)
        {
            auto e= dynamic_cast<QMouseEvent*>(event);
            if(e)
            {
                auto index= ui->m_antogonistView->indexAt(e->pos());
            }
        }*/
    }

    return QWidget::eventFilter(obj, event);
}

bool AntagonistBoard::fullMode() const
{
    return m_fullModeAct->isChecked();
}

void AntagonistBoard::setFullMode(bool newFullMode)
{
    if(fullMode() == newFullMode)
        return;
    m_fullModeAct->setChecked(newFullMode);
    emit fullModeChanged();
}

bool AntagonistBoard::minimalMode() const
{
    return m_minimalModeAct->isChecked();
}

void AntagonistBoard::setMinimalMode(bool newMinimalMode)
{
    if(minimalMode() == newMinimalMode)
        return;
    m_minimalModeAct->setChecked(newMinimalMode);
    emit minimalModeChanged();
}

} // namespace campaign
