#include "selectconnectionprofiledialog.h"
#include "ui_selectconnectionprofiledialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>

#include "data/character.h"

#include "controller/gamecontroller.h"
#include "controller/networkcontroller.h"
#include "controller/playercontroller.h"

#define DEFAULT_PORT 6660

/// ConnectionProfile
SelectConnectionProfileDialog::SelectConnectionProfileDialog(GameController* ctrl, QWidget* parent)
    : QDialog(parent), ui(new Ui::SelectConnectionProfileDialog), m_ctrl(ctrl)
{
    ui->setupUi(this);
    m_model= m_ctrl->networkController()->profileModel();
    ui->m_profileList->setModel(m_model);
    ui->m_progressBar->setVisible(false);

    connect(ui->m_profileList->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex& selected, const QModelIndex&) { setCurrentProfile(selected); });
    connect(ui->m_profileList, &QListView::doubleClicked, this, &SelectConnectionProfileDialog::connectToIndex);

    ui->m_profileList->setCurrentIndex(m_model->index(0, 0));
    setCurrentProfile(ui->m_profileList->currentIndex());

    connect(ui->m_passwordEdit, &QLineEdit::textEdited, this, [this]() { m_passChanged= true; });

    connect(ui->m_addProfile, &QPushButton::clicked, m_ctrl->networkController(), &NetworkController::appendProfile);
    connect(ui->m_cancel, &QPushButton::clicked, this, &SelectConnectionProfileDialog::reject);
    connect(ui->m_connect, &QPushButton::clicked, this, &SelectConnectionProfileDialog::connectTo);
    connect(ui->m_selectCharaterAvatar, &QToolButton::clicked, this, &SelectConnectionProfileDialog::openImage);
    connect(ui->m_delProfileAct, &QPushButton::clicked, this, &SelectConnectionProfileDialog::removeProfile);
    connect(ui->m_addresseLineEdit, &QLineEdit::textChanged, this, &SelectConnectionProfileDialog::checkConnection);
    connect(ui->m_isServerCheckbox, &QCheckBox::toggled, this, &SelectConnectionProfileDialog::checkConnection);
    connect(m_ctrl->networkController(), &NetworkController::connectingChanged, ui->m_progressBar,
            &QProgressBar::setVisible);
    connect(m_ctrl->networkController(), &NetworkController::connectingChanged, this,
            [this](bool connecting) { ui->m_connect->setEnabled(!connecting); });
}

SelectConnectionProfileDialog::~SelectConnectionProfileDialog()
{
    delete ui;
}

void SelectConnectionProfileDialog::setCurrentProfile(QModelIndex index)
{
    if(m_currentProfileIndex == index.row())
        return;
    m_ctrl->networkController()->stopConnecting();
    m_currentProfileIndex= index.row();
    updateProfile();
    m_avatarUri.clear();
    updateGUI();
    ui->m_connect->setEnabled(true);
}
void SelectConnectionProfileDialog::updateGUI()
{
    auto profile= m_ctrl->networkController()->getProfile(m_currentProfileIndex);

    if(profile == nullptr)
        return;

    ui->m_addresseLineEdit->setText(profile->getAddress());
    ui->m_name->setText(profile->getName());
    ui->m_profileTitle->setText(profile->getTitle());
    ui->m_port->setValue(profile->getPort());
    ui->m_isServerCheckbox->setChecked(profile->isServer());
    ui->m_isGmCheckbox->setChecked(profile->isGM());
    ui->m_addresseLineEdit->setEnabled(!profile->isServer());
    ui->m_colorBtn->setColor(profile->getPlayer()->getColor());
    m_passChanged= false;
    ui->m_passwordEdit->setText(profile->getPassword());

    if(nullptr != profile->getCharacter())
    {
        ui->m_characterName->setText(profile->getCharacter()->name());
        ui->m_characterColor->setColor(profile->getCharacter()->getColor());
        ui->m_selectCharaterAvatar->setIcon(QIcon(QPixmap::fromImage(profile->getCharacter()->getAvatar())));
    }
    else
    {
        ui->m_selectCharaterAvatar->setIcon(QIcon());
    }
}
void SelectConnectionProfileDialog::removeProfile()
{
    if(-1 == m_currentProfileIndex)
        return;

    auto profile= m_ctrl->networkController()->getProfile(m_currentProfileIndex);

    if(QMessageBox::No
       == QMessageBox::question(
              this, tr("Remove Current Profile"),
              tr("Do you really want to remove %1 from your connection list ?").arg(profile->getTitle())))
    {
        return;
    }

    m_ctrl->networkController()->removeProfile(m_currentProfileIndex);

    int size= m_model->rowCount(QModelIndex());
    if(size <= m_currentProfileIndex)
    {
        m_currentProfileIndex= size - 1;
    }
    updateGUI();
}

void SelectConnectionProfileDialog::updateProfile()
{
    ui->m_errorNotification->setStyleSheet("");
    auto networkCtrl= m_ctrl->networkController();
    networkCtrl->setHost(ui->m_addresseLineEdit->text());
    networkCtrl->setPort(ui->m_port->value());
    networkCtrl->setHosting(ui->m_isServerCheckbox->isChecked());
    networkCtrl->setIsGM(ui->m_isGmCheckbox->isChecked());
    networkCtrl->setAskForGM(ui->m_isGmCheckbox->isChecked());
    if(m_passChanged)
        networkCtrl->setServerPassword(ui->m_passwordEdit->text().toUtf8());

    auto playerCtrl= m_ctrl->playerController();
    auto localPlayer= playerCtrl->localPlayer();
    localPlayer->setColor(ui->m_colorBtn->color());
    localPlayer->setName(ui->m_name->text());
    localPlayer->setGM(ui->m_isGmCheckbox->isChecked());

    /*    if(nullptr != m_currentProfile)
        {
            m_currentProfile->setAddress(ui->m_addresseLineEdit->text());
            m_currentProfile->setName(ui->m_name->text());
            m_currentProfile->setPort(ui->m_port->value());
            m_currentProfile->setServerMode(ui->m_isServerCheckbox->isChecked());
            m_currentProfile->setTitle(ui->m_profileTitle->text());
            m_currentProfile->setGm(ui->m_isGmCheckbox->isChecked());
            if(m_passChanged)
            {
                m_currentProfile->setPassword(ui->m_passwordEdit->text());
            }
            // TODO the profile must do that:
            auto player= m_currentProfile->getPlayer();
            player->setColor(ui->m_colorBtn->color());
            player->setName(ui->m_name->text());
            player->setGM(ui->m_isGmCheckbox->isChecked());

            Character* character= m_currentProfile->getCharacter();
            if(character != nullptr)
            {
                if(!m_avatarUri.isEmpty())
                {
                    character->setAvatarPath(m_avatarUri);
                    character->setAvatar(QImage(m_avatarUri));
                }
                character->setName(ui->m_characterName->text());
                character->setColor(ui->m_characterColor->color());
            }
        }*/
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

void SelectConnectionProfileDialog::connectToIndex(QModelIndex index)
{
    m_currentProfileIndex= index.row();
    updateGUI();
    connectTo();
}
void SelectConnectionProfileDialog::connectTo()
{
    ui->m_connect->setEnabled(false);
    updateProfile();
    m_ctrl->startConnection(m_currentProfileIndex);
}
void SelectConnectionProfileDialog::openImage()
{
    m_avatarUri= QFileDialog::getOpenFileName(this, tr("Load Avatar"));

    updateProfile();
    updateGUI();
}
void SelectConnectionProfileDialog::checkConnection()
{
    bool valid= false;
    if((!ui->m_addresseLineEdit->text().isEmpty()) && (!ui->m_isServerCheckbox->isChecked()))
    {
        valid= true;
    }
    else if(ui->m_isServerCheckbox->isChecked())
    {
        valid= true;
    }
    ui->m_connect->setEnabled(valid);
}

void SelectConnectionProfileDialog::errorOccurs(QString str)
{
    ui->m_errorNotification->setStyleSheet("font: 19pt ;\nbackground: rgb(255, 0, 0);\ncolor: rgb(0,0,0);");
    ui->m_errorNotification->setText(str);
}
