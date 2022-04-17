#include "selectconnectionprofiledialog.h"
#include "ui_selectconnectionprofiledialog.h"

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <QQmlEngine>

#include "data/character.h"
#include "worker/iohelper.h"

#include "controller/gamecontroller.h"
#include "controller/networkcontroller.h"
#include "controller/playercontroller.h"
#include "controller/view_controller/imageselectorcontroller.h"
#include "controller/view_controller/selectconnprofilecontroller.h"
#include "imageselectordialog.h"
#include "model/profilemodel.h"

#include "qml_views/image_selector.h"

/// ConnectionProfile
SelectConnectionProfileDialog::SelectConnectionProfileDialog(GameController* ctrl, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::SelectConnectionProfileDialog)
    , m_gameCtrl{ctrl}
    , m_ctrl{new SelectConnProfileController(ctrl->networkController()->profileModel(), ctrl)}
{
    ui->setupUi(this);
    // ui->m_detailPanel->setVisible(false);
    connect(ui->m_quickWidget, &QQuickWidget::sceneGraphError, this,
            [](QQuickWindow::SceneGraphError, const QString& message) { qDebug() << "ERROR" << message; });
    connect(ui->m_quickWidget, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if(status == QQuickWidget::Error)
            qDebug() << "Errors: " << ui->m_quickWidget->errors();
    });

    /*m_model= m_ctrl->networkController()->profileModel();
    ui->m_profileList->setModel(m_model);
    ui->m_progressBar->setVisible(false);

    ui->m_rootDirCampEdit->setPath(QDir::homePath());
    ui->m_rootDirCampEdit->setMode(true);

    connect(ui->m_profileList->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex& selected, const QModelIndex&) {
                setCurrentProfile(selected);
                ui->m_cloneProfileAct->setEnabled(selected.isValid());
            });
    connect(ui->m_profileList, &QListView::doubleClicked, this, &SelectConnectionProfileDialog::connectToIndex);
    connect(ui->m_playerAvatarAct, &QAction::triggered, this, &SelectConnectionProfileDialog::selectPlayerAvatar);
    connect(ui->m_cloneProfileAct, &QAction::triggered, this, &SelectConnectionProfileDialog::cloneProfile);
    connect(m_ctrl, &GameController::dataLoaded, this, [this]() { setState(State::LOADED); });

    ui->m_avatarPlayer->setDefaultAction(ui->m_playerAvatarAct);

    ui->m_profileList->setCurrentIndex(m_model->index(0, 0));
    setCurrentProfile(ui->m_profileList->currentIndex());
    ui->m_addCharacterBtn->setDefaultAction(ui->m_addCharacterAct);
    ui->m_cloneProfileBtn->setDefaultAction(ui->m_cloneProfileAct);
    ui->m_removeCharacterBtn->setDefaultAction(ui->m_removeCharacterAct);
    ui->m_characterList->setModel(m_characterModel.get());
    auto vh= ui->m_characterList->horizontalHeader();
    vh->resizeSection(0, 50);

    connect(ui->m_addCharacterAct, &QAction::triggered, m_characterModel.get(), &CharacterDataModel::insertCharacter);
    connect(ui->m_removeCharacterAct, &QAction::triggered, this, [this]() {
        auto idx= ui->m_characterList->currentIndex();
        if(idx.isValid())
            m_characterModel->removeCharacter(idx);
    });*/

    qRegisterMetaType<ProfileModel*>("ProfileModel*");
    qRegisterMetaType<CharacterDataModel*>("CharacterDataModel*");
    qmlRegisterSingletonInstance<SelectConnProfileController>("Profile", 1, 0, "ProfileController", m_ctrl.get());
    qmlRegisterSingletonInstance<SelectConnectionProfileDialog>("Profile", 1, 0, "ProfileView", this);
    qmlRegisterType<ImageSelector>("Profile", 1, 0, "ImageSelector");
    auto engine= ui->m_quickWidget->engine();
    engine->addImportPath(QStringLiteral("qrc:/qml"));

    ui->m_quickWidget->setSource(QUrl("qrc:/qml/views/ConnectionForm.qml"));
    ui->m_quickWidget->setVisible(true);

    // link between game controller and profile controller
    auto networkCtrl= m_gameCtrl->networkController();
    connect(networkCtrl, &NetworkController::lastErrorChanged, m_ctrl.get(), &SelectConnProfileController::setErrorMsg);
    connect(networkCtrl, &NetworkController::infoMessage, m_ctrl.get(), &SelectConnProfileController::setInfoMsg);
    connect(networkCtrl, &NetworkController::connectingChanged, this, [this](bool connecting) {
        m_ctrl->setConnectionState(connecting ? SelectConnProfileController::ConnectionState::CONNECTING :
                                                SelectConnProfileController::ConnectionState::IDLE);
    });
    connect(m_ctrl.get(), &SelectConnProfileController::startConnect, networkCtrl, &NetworkController::startConnection);
    connect(m_ctrl.get(), &SelectConnProfileController::stopConnecting, networkCtrl,
            &NetworkController::stopConnecting);
    connect(ctrl, &GameController::dataLoaded, this,
            [this]() { m_ctrl->setConnectionState(SelectConnProfileController::ConnectionState::LOADED); });
}

SelectConnectionProfileDialog::~SelectConnectionProfileDialog()
{
    delete ui;
}

void SelectConnectionProfileDialog::setArgumentProfile(QString host, int port, QByteArray password)
{
    /*  ConnectionProfile* fromURL= new ConnectionProfile();
      fromURL->setTitle(tr("From URL"));
      fromURL->setName(tr("Unknown"));
      fromURL->setAddress(host);
      fromURL->setPort(port);
      fromURL->setHash(password);
      fromURL->setGm(false);
      fromURL->setServerMode(false);
      fromURL->setPlayer(new Player);
      m_model->appendProfile(fromURL);
      auto index= m_model->indexOf(fromURL);
      ui->m_profileList->setCurrentIndex(m_model->index(index, 0));
      m_currentProfile= fromURL;
      updateGUI();
      updateProfile();*/
}

QByteArray SelectConnectionProfileDialog::openImage(const QString& path)
{
    QFileInfo info(path);
    ImageSelectorController ctrl(true, ImageSelectorController::All, ImageSelectorController::Square,
                                 info.absoluteFilePath());
    ImageSelectorDialog dialog(&ctrl, this);

    if(QDialog::Accepted != dialog.exec())
        return {};

    return ctrl.finalImageData();
}

void SelectConnectionProfileDialog::selectPlayerAvatar()
{
    auto data= openImage(m_ctrl->playerAvatar());

    if(data.isEmpty())
        return;

    m_ctrl->setPlayerAvatar(data);
    ui->m_playerAvatarAct->setIcon(QIcon(IOHelper::dataToPixmap(data)));
}

void SelectConnectionProfileDialog::selectCampaignPath()
{

    auto result= QFileDialog::getExistingDirectory(this, tr("Select directory"), m_ctrl->campaignPath(),
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if(!result.isEmpty())
    {
        m_ctrl->setCampaignPath(result);
    }
}

void SelectConnectionProfileDialog::startNetworkConnection()
{
    m_ctrl->setConnectionState(SelectConnProfileController::ConnectionState::LOADING);
    m_gameCtrl->setDataFromProfile(m_ctrl->currentProfileIndex());
}

void SelectConnectionProfileDialog::selectCharacterAvatar(int i)
{
    auto characters= m_ctrl->characterModel();
    auto character= characters->character(i);

    auto data= openImage(m_ctrl->playerAvatar());

    if(data.isEmpty())
        return;

    characters->setAvatar(i, data);
}
