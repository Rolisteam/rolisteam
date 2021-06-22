#include "selectconnectionprofiledialog.h"
#include "ui_selectconnectionprofiledialog.h"

#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>

#include "data/character.h"

#include "controller/gamecontroller.h"
#include "controller/networkcontroller.h"
#include "controller/playercontroller.h"

#include "model/profilemodel.h"
#include "network/characterdatamodel.h"

/// ConnectionProfile
SelectConnectionProfileDialog::SelectConnectionProfileDialog(GameController* ctrl, QWidget* parent)
    : QDialog(parent), ui(new Ui::SelectConnectionProfileDialog), m_ctrl(ctrl), m_characterModel(new CharacterDataModel)
{
    ui->setupUi(this);
    m_model= m_ctrl->networkController()->profileModel();
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
    });

    connect(ui->m_characterList, &QTableView::doubleClicked, this, [this](const QModelIndex& index) {
        if(index.column() == 0)
        {
            auto img= openImage(index.data(Qt::EditRole).toString());
            if(!img.isEmpty())
                m_characterModel->setData(index, img);
        }
        else if(index.column() == 1)
        {
            auto color= QColorDialog::getColor(index.data(Qt::BackgroundRole).value<QColor>());
            if(color.isValid())
                m_characterModel->setData(index, color);
        }
    });

    connect(ui->m_passwordEdit, &QLineEdit::textEdited, this, [this]() { m_passChanged= true; });

    connect(ui->m_addProfile, &QPushButton::clicked, this, [this]() { m_model->appendProfile(); });
    connect(ui->m_cancel, &QPushButton::clicked, this, &SelectConnectionProfileDialog::reject);
    connect(ui->m_connectAct, &QAction::triggered, this, &SelectConnectionProfileDialog::connectTo);
    connect(ui->m_stopConnectAct, &QAction::triggered, this, &SelectConnectionProfileDialog::stopConnecting);
    connect(ui->m_delProfileAct, &QPushButton::clicked, this, &SelectConnectionProfileDialog::removeProfile);
    connect(ui->m_addresseLineEdit, &QLineEdit::textChanged, this, &SelectConnectionProfileDialog::checkConnection);
    connect(ui->m_isServerCheckbox, &QCheckBox::toggled, this, &SelectConnectionProfileDialog::checkConnection);
    connect(m_ctrl->networkController(), &NetworkController::connectingChanged, ui->m_progressBar,
            &QProgressBar::setVisible);
    connect(m_ctrl->networkController(), &NetworkController::connectingChanged, this, [this](bool connecting) {
        if(connecting)
        {
            ui->m_connectBtn->setDefaultAction(ui->m_stopConnectAct);
            ui->m_connectBtn->removeAction(ui->m_connectAct);
        }
        else
        {
            ui->m_connectBtn->setDefaultAction(ui->m_connectAct);
            ui->m_connectBtn->removeAction(ui->m_stopConnectAct);
        }
    });

    ui->m_connectBtn->setDefaultAction(ui->m_connectAct);
}

SelectConnectionProfileDialog::~SelectConnectionProfileDialog()
{
    delete ui;
}

void SelectConnectionProfileDialog::setCurrentProfile(QModelIndex index)
{
    qDebug() << "setCurrentProfile" << index.row();
    if(m_currentProfileIndex == index.row())
        return;
    m_ctrl->networkController()->stopConnecting();
    updateProfile();
    m_currentProfileIndex= index.row();
    updateGUI();
    //    updateProfile();
    m_avatarUri.clear();
    ui->m_connectAct->setEnabled(true);
}

void SelectConnectionProfileDialog::stopConnecting()
{
    m_ctrl->stopConnection();
}

void SelectConnectionProfileDialog::updateProfile()
{
    qDebug() << "updateProfile" << m_currentProfileIndex;
    ui->m_errorNotification->setStyleSheet("");

    auto profile= m_model->getProfile(m_currentProfileIndex);

    if(nullptr != profile)
    {
        profile->setAddress(ui->m_addresseLineEdit->text());
        profile->setPort(ui->m_port->value());
        profile->setServerMode(ui->m_isServerCheckbox->isChecked());
        profile->setProfileTitle(ui->m_profileTitle->text());
        profile->setGm(ui->m_isGmCheckbox->isChecked());
        if(ui->m_isGmCheckbox->isChecked())
            profile->setCampaignPath(ui->m_rootDirCampEdit->path());
        if(m_passChanged)
        {
            profile->setPassword(ui->m_passwordEdit->text());
        }

        profile->setPlayerColor(ui->m_colorBtn->color());
        profile->setPlayerName(ui->m_name->text());
        // profile->setPlayerAvatar(ui->m_isGmCheckbox->isChecked());

        /*Character* character= profile->getCharacter();
        if(character != nullptr)
        {
            if(!m_avatarUri.isEmpty())
            {
                character->setAvatarPath(m_avatarUri);
                character->setAvatar(QImage(m_avatarUri));
            }
            character->setName(ui->m_characterName->text());
            character->setColor(ui->m_characterColor->color());
        }*/
    }
}

void SelectConnectionProfileDialog::updateGUI()
{
    qDebug() << "updateGUI" << m_currentProfileIndex;
    auto profile= m_model->getProfile(m_currentProfileIndex);

    if(profile == nullptr)
        return;

    ui->m_addresseLineEdit->setText(profile->address());
    ui->m_name->setText(profile->playerName());
    ui->m_profileTitle->setText(profile->profileTitle());
    ui->m_port->setValue(profile->port());
    ui->m_isServerCheckbox->setChecked(profile->isServer());
    ui->m_isGmCheckbox->setChecked(profile->isGM());
    ui->m_addresseLineEdit->setEnabled(!profile->isServer());
    ui->m_colorBtn->setColor(profile->playerColor());
    m_passChanged= false;
    ui->m_passwordEdit->setText(profile->password());
    ui->m_playerAvatarAct->setIcon(QIcon(profile->playerAvatar()));
    ui->m_rootDirCampEdit->setPath(profile->campaignPath());

    m_characterModel->setProfile(profile);
    /*if(!profile->isGM())
    {
        ui->m_characterName->setText(profile->getCharacter()->name());
        ui->m_characterColor->setColor(profile->getCharacter()->getColor());
        ui->m_selectCharaterAvatar->setIcon(QIcon(QPixmap::fromImage(profile->getCharacter()->getAvatar())));
    }
    else
    {
        ui->m_selectCharaterAvatar->setIcon(QIcon());
    }*/
}
void SelectConnectionProfileDialog::removeProfile()
{
    if(-1 == m_currentProfileIndex)
        return;

    auto profile= m_model->getProfile(m_currentProfileIndex);

    if(QMessageBox::No
       == QMessageBox::question(
              this, tr("Remove Current Profile"),
              tr("Do you really want to remove %1 from your connection list ?").arg(profile->profileTitle())))
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
    updateProfile();
    auto networkCtrl= m_ctrl->networkController();
    networkCtrl->setHost(ui->m_addresseLineEdit->text());
    networkCtrl->setPort(ui->m_port->value());
    networkCtrl->setHosting(ui->m_isServerCheckbox->isChecked());
    networkCtrl->setIsGM(ui->m_isGmCheckbox->isChecked());
    networkCtrl->setAskForGM(ui->m_isGmCheckbox->isChecked());
    if(m_passChanged)
        networkCtrl->setServerPassword(ui->m_passwordEdit->text().toUtf8());

    auto profile= m_model->getProfile(m_currentProfileIndex);

    auto playerCtrl= m_ctrl->playerController();
    auto localPlayer= playerCtrl->localPlayer();
    if(!localPlayer) // WARNING
        return;
    localPlayer->setUuid(profile->playerId());
    localPlayer->setColor(ui->m_colorBtn->color());
    localPlayer->setName(ui->m_name->text());
    localPlayer->setGM(ui->m_isGmCheckbox->isChecked());

    m_ctrl->startConnection(m_currentProfileIndex);
}

void SelectConnectionProfileDialog::cloneProfile()
{
    auto idx= m_model->cloneProfile(ui->m_profileList->currentIndex());
    if(idx < 0)
        return;

    ui->m_profileList->setCurrentIndex(m_model->index(idx, 0));
}

QString SelectConnectionProfileDialog::openImage(const QString& path)
{
    QFileInfo info(path);
    return QFileDialog::getOpenFileName(this, tr("Load Avatar"), info.absolutePath());
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
    ui->m_connectAct->setEnabled(valid);
}

void SelectConnectionProfileDialog::errorOccurs(QString str)
{
    ui->m_errorNotification->setStyleSheet("font: 19pt ;\nbackground: rgb(255, 0, 0);\ncolor: rgb(0,0,0);");
    ui->m_errorNotification->setText(str);
}

void SelectConnectionProfileDialog::selectPlayerAvatar()
{
    if(-1 == m_currentProfileIndex)
        return;

    auto profile= m_model->getProfile(m_currentProfileIndex);

    auto newpath= openImage(profile->playerAvatar());

    if(newpath.isEmpty())
        return;

    QImage img(newpath);

    if(img.height() != img.width())
    { // not square
      // TODO chop picture if it is not square.
    }

    profile->setPlayerAvatar(newpath);
    ui->m_playerAvatarAct->setIcon(QIcon(newpath));
}
