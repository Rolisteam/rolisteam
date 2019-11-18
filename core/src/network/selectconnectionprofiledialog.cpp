#include "selectconnectionprofiledialog.h"
#include "ui_selectconnectionprofiledialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QImage>
#include <QMessageBox>

#include "data/character.h"

#define DEFAULT_PORT 6660

/// ConnectionProfile
/////////////////////////////////
///
/// SelectConnectionProfileDialog
///
////////////////////////////////

SelectConnectionProfileDialog::SelectConnectionProfileDialog(QString version, QWidget* parent)
    : QDialog(parent), ui(new Ui::SelectConnectionProfileDialog), m_currentProfile(nullptr), m_version(version)
{
    ui->setupUi(this);
    m_model= new ProfileModel(m_version);
    readSettings();
    ui->m_profileList->setModel(m_model);
    ui->m_progressBar->setVisible(false);

    connect(ui->m_profileList->selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this](const QModelIndex& selected, const QModelIndex&) { setCurrentProfile(selected); });
    connect(ui->m_profileList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(connectToIndex(QModelIndex)));

    ui->m_profileList->setCurrentIndex(m_model->index(0, 0));
    setCurrentProfile(ui->m_profileList->currentIndex());

    connect(ui->m_passwordEdit, &QLineEdit::textEdited, this, [=]() { m_passChanged= true; });

    connect(ui->m_addProfile, SIGNAL(clicked()), m_model, SLOT(appendProfile()));
    connect(ui->m_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->m_connect, &QPushButton::clicked, this, &SelectConnectionProfileDialog::connectTo);
    connect(ui->m_selectCharaterAvatar, SIGNAL(clicked()), this, SLOT(openImage()));
    connect(ui->m_delProfileAct, SIGNAL(pressed()), this, SLOT(removeProfile()));
    connect(ui->m_addresseLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConnection()));
    connect(ui->m_isServerCheckbox, SIGNAL(toggled(bool)), this, SLOT(checkConnection()));
}

SelectConnectionProfileDialog::~SelectConnectionProfileDialog()
{
    delete ui;
}

ConnectionProfile* SelectConnectionProfileDialog::getSelectedProfile()
{
    return m_currentProfile;
}
void SelectConnectionProfileDialog::setCurrentProfile(QModelIndex index)
{
    updateProfile();
    m_currentProfile= m_model->getProfile(index);
    m_avatarUri.clear();
    updateGUI();
}
void SelectConnectionProfileDialog::updateGUI()
{
    if(nullptr != m_currentProfile)
    {
        ui->m_addresseLineEdit->setText(m_currentProfile->getAddress());
        ui->m_name->setText(m_currentProfile->getName());
        ui->m_profileTitle->setText(m_currentProfile->getTitle());
        ui->m_port->setValue(m_currentProfile->getPort());
        ui->m_isServerCheckbox->setChecked(m_currentProfile->isServer());
        ui->m_isGmCheckbox->setChecked(m_currentProfile->isGM());
        ui->m_addresseLineEdit->setEnabled(!m_currentProfile->isServer());
        ui->m_colorBtn->setColor(m_currentProfile->getPlayer()->getColor());
        m_passChanged= false;
        ui->m_passwordEdit->setText(m_currentProfile->getPassword());

        if(nullptr != m_currentProfile->getCharacter())
        {
            ui->m_characterName->setText(m_currentProfile->getCharacter()->name());
            ui->m_characterColor->setColor(m_currentProfile->getCharacter()->getColor());
            ui->m_selectCharaterAvatar->setIcon(
                QIcon(QPixmap::fromImage(m_currentProfile->getCharacter()->getAvatar())));
        }
        else
        {
            ui->m_selectCharaterAvatar->setIcon(QIcon());
        }
    }
}
void SelectConnectionProfileDialog::removeProfile()
{
    if(nullptr != m_currentProfile)
    {
        if(QMessageBox::No
           == QMessageBox::question(
                  this, tr("Remove Current Profile"),
                  tr("Do you really want to remove %1 from your connection list ?").arg(m_currentProfile->getTitle())))
        {
            return;
        }
        int i= m_model->indexOf(m_currentProfile);
        m_model->removeProfile(m_currentProfile);
        int size= m_model->rowCount(QModelIndex());
        if(size > i)
        {
            m_currentProfile= m_model->getProfile(i);
        }
        else if(size > 0)
        {
            m_currentProfile= m_model->getProfile(size - 1);
        }
        else
        {
            m_currentProfile= nullptr;
        }
        updateGUI();
    }
}

void SelectConnectionProfileDialog::updateProfile()
{
    ui->m_errorNotification->setStyleSheet("");
    if(nullptr != m_currentProfile)
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
    }
}
void SelectConnectionProfileDialog::readSettings()
{
    m_model->readSettings();
}

void SelectConnectionProfileDialog::writeSettings()
{
    m_model->writeSettings();
}

void SelectConnectionProfileDialog::setArgumentProfile(QString host, int port, QByteArray password)
{
    ConnectionProfile* fromURL= new ConnectionProfile();
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
    updateProfile();
}

void SelectConnectionProfileDialog::connectToIndex(QModelIndex index)
{
    m_currentProfile= m_model->getProfile(index);
    updateGUI();
    connectTo();
}
void SelectConnectionProfileDialog::connectTo()
{
    ui->m_connect->blockSignals(true);
    updateProfile();
    ui->m_progressBar->setVisible(true);
    emit tryConnection();
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
    ui->m_progressBar->setVisible(false);
    endOfConnectionProcess();
}

void SelectConnectionProfileDialog::disconnection()
{
    ui->m_progressBar->setVisible(false);
    endOfConnectionProcess();
}

void SelectConnectionProfileDialog::endOfConnectionProcess()
{
    ui->m_connect->blockSignals(false);
}
