#include "selectconnectionprofiledialog.h"
#include "ui_selectconnectionprofiledialog.h"

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <QQmlEngine>

#include "worker/iohelper.h"

#include "controller/gamecontroller.h"
#include "controller/networkcontroller.h"
#include "controller/view_controller/imageselectorcontroller.h"
#include "controller/view_controller/selectconnprofilecontroller.h"
#include "imageselectordialog.h"
#include "model/profilemodel.h"

#include "qml_views/image_selector.h"

/// ConnectionProfile
SelectConnectionProfileDialog::SelectConnectionProfileDialog(GameController* ctrl, QWidget* parent)
    : QDialog(parent), ui(new Ui::SelectConnectionProfileDialog), m_gameCtrl{ctrl}
{
    if(ctrl && ctrl->networkController() && ctrl->networkController()->profileModel())
        m_ctrl.reset(new SelectConnProfileController(ctrl->networkController()->profileModel(), ctrl));

    ui->setupUi(this);
    connect(ui->m_quickWidget, &QQuickWidget::sceneGraphError, this,
            [](QQuickWindow::SceneGraphError, const QString& message) { qDebug() << "ERROR" << message; });
    connect(ui->m_quickWidget, &QQuickWidget::statusChanged, this,
            [this](QQuickWidget::Status status)
            {
                if(status == QQuickWidget::Error)
                    qDebug() << "Errors: " << ui->m_quickWidget->errors();
            });

    qRegisterMetaType<ProfileModel*>("ProfileModel*");
    qRegisterMetaType<CharacterDataModel*>("CharacterDataModel*");
    qmlRegisterSingletonInstance<SelectConnProfileController>("Profile", 1, 0, "ProfileController", m_ctrl.get());
    // qmlRegisterSingletonInstance<SelectConnectionProfileDialog>("Profile", 1, 0, "ProfileView", this);

    qmlRegisterType<ImageSelector>("Profile", 1, 0, "ImageSelector");
    auto engine= ui->m_quickWidget->engine();
    engine->setObjectOwnership(m_ctrl.get(), QQmlEngine::CppOwnership);
    engine->addImportPath(QStringLiteral("qrc:/qml"));

    ui->m_quickWidget->setSource(QUrl("qrc:/qml/views/ConnectionForm.qml"));
    ui->m_quickWidget->setVisible(true);

    // link between game controller and profile controller
    if(!m_gameCtrl)
        return;
    auto networkCtrl= m_gameCtrl->networkController();
    connect(networkCtrl, &NetworkController::lastErrorChanged, m_ctrl.get(), &SelectConnProfileController::setErrorMsg);
    connect(networkCtrl, &NetworkController::infoMessage, m_ctrl.get(), &SelectConnProfileController::setInfoMsg);
    connect(networkCtrl, &NetworkController::connectingChanged, this,
            [this](bool connecting)
            {
                m_ctrl->setConnectionState(connecting ? SelectConnProfileController::ConnectionState::CONNECTING :
                                                        SelectConnProfileController::ConnectionState::IDLE);
            });

    // actions from controller
    connect(m_ctrl.get(), &SelectConnProfileController::saveModels, networkCtrl, &NetworkController::saveData);
    connect(m_ctrl.get(), &SelectConnProfileController::connectionStarted, this,
            [this]() { m_gameCtrl->setDataFromProfile(m_ctrl->currentProfileIndex()); });
    connect(m_ctrl.get(), &SelectConnProfileController::portChanged, this,
            [this]()
            {
                if(!m_gameCtrl)
                    return;
                auto networkCtrl= m_gameCtrl->networkController();
                if(!networkCtrl)
                    return;
                auto profile= networkCtrl->currentProfile();
                if(!profile)
                    return;

                profile->setPort(m_ctrl->port());
            });
    connect(m_ctrl.get(), &SelectConnProfileController::addressChanged, this,
            [this]()
            {
                if(!m_gameCtrl)
                    return;
                auto networkCtrl= m_gameCtrl->networkController();
                if(!networkCtrl)
                    return;
                auto profile= networkCtrl->currentProfile();
                if(!profile)
                    return;

                profile->setAddress(m_ctrl->address());
            });
    connect(m_ctrl.get(), &SelectConnProfileController::passwordChanged, this,
            [this]()
            {
                if(!m_gameCtrl)
                    return;
                auto networkCtrl= m_gameCtrl->networkController();
                if(!networkCtrl)
                    return;
                auto profile= networkCtrl->currentProfile();
                if(!profile)
                    return;

                profile->editPassword(m_ctrl->password());
            });
    connect(m_ctrl.get(), &SelectConnProfileController::rejected, this, &SelectConnectionProfileDialog::reject);
    connect(m_ctrl.get(), &SelectConnProfileController::changeCampaignPath, this,
            [this]()
            {
                auto result
                    = QFileDialog::getExistingDirectory(this, tr("Select directory"), m_ctrl->campaignPath(),
                                                        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
                if(!result.isEmpty())
                {
                    m_ctrl->setCampaignPath(result);
                }
            });
    connect(m_ctrl.get(), &SelectConnProfileController::changeCharacterAvatar, this,
            [this](int i)
            {
                auto characters= m_ctrl->characterModel();
                auto character= characters->character(i);
                auto data= openImage(); // m_ctrl->playerAvatar()
                if(data.isEmpty())
                    return;
                characters->setAvatar(i, data);
            });
    connect(m_ctrl.get(), &SelectConnProfileController::changePlayerAvatar, this,
            [this]()
            {
                auto data= openImage(); // m_ctrl->playerAvatar()

                if(data.isEmpty())
                    return;

                m_ctrl->setPlayerAvatar(data);
                ui->m_playerAvatarAct->setIcon(QIcon(IOHelper::dataToPixmap(data)));
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
    qDebug() << "path" << path;
    QFileInfo info(path);
    ImageSelectorController ctrl(true, ImageSelectorController::All, ImageSelectorController::Square,
                                 info.absoluteFilePath());
    ImageSelectorDialog dialog(&ctrl, this);

    if(QDialog::Accepted != dialog.exec())
        return {};

    ctrl.address();

    return ctrl.finalImageData();
}
