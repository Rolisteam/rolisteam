#include "selectconnectionprofiledialog.h"
#include "ui_selectconnectionprofiledialog.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>


#include "data/character.h"

#define DEFAULT_PORT 6660

/// ConnectionProfile
///
///
ConnectionProfile::ConnectionProfile()
    : m_character(NULL),m_server(false),m_port(DEFAULT_PORT),m_player(NULL),m_isGM(false)
{

}
ConnectionProfile::~ConnectionProfile()
{

}
void ConnectionProfile::setTitle(QString str)
{
    m_title = str;
}
void ConnectionProfile::setName(QString str)
{
    m_name = str;
}
void ConnectionProfile::setAddress(QString str)
{
    m_address = str;
}
void ConnectionProfile::setPort(int i)
{
    m_port = i;
}
void ConnectionProfile::setServerMode(bool b)
{
    m_server = b;
}
void ConnectionProfile::setPlayer(Player* p)
{
    m_player = p;
}
void ConnectionProfile::setGm(bool b)
{
    m_isGM = b;
}
QString ConnectionProfile::getTitle()const
{
    return m_title;
}
QString ConnectionProfile::getName() const
{
    return m_name;
}
QString ConnectionProfile::getAddress() const
{
    return m_address;
}
int ConnectionProfile::getPort() const
{
    return m_port;
}
bool ConnectionProfile::isServer() const
{
    return m_server;
}
Player* ConnectionProfile::getPlayer() const
{
    return m_player;
}
bool    ConnectionProfile::isGM() const
{
    return m_isGM;
}
Character* ConnectionProfile::getCharacter() const
{
    return m_character;
}
void ConnectionProfile::setCharacter(Character* character)
{
    m_character = character;
}

////////////
//model
////////////
ProfileModel::ProfileModel(QString version)
    :m_version(version)
{

}

ProfileModel::~ProfileModel()
{
    qDeleteAll(m_connectionProfileList);
    m_connectionProfileList.clear();
}
QVariant ProfileModel::headerData(int section, Qt::Orientation orientation, int role)
{
    return QVariant();
}

int ProfileModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
    {
        return m_connectionProfileList.size();
    }
    else
    {
        return 0;
    }
}
QVariant ProfileModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(Qt::DisplayRole == role)
    {

        return m_connectionProfileList.at(index.row())->getTitle();
    }

    return QVariant();
}

void ProfileModel::appendProfile()
{
    ConnectionProfile* profile = new ConnectionProfile();
    profile->setTitle(QStringLiteral("Profile #%1").arg(m_connectionProfileList.size()+1));
    profile->setPort(6660);
    profile->setName(QStringLiteral("Unknown User"));
    Player* player = new Player();
    player->setUserVersion(m_version);
    profile->setPlayer(player);
    profile->setCharacter(new Character(QStringLiteral("Unknown"),Qt::black,false));
    beginInsertRows(QModelIndex(),m_connectionProfileList.size(),m_connectionProfileList.size());
    m_connectionProfileList.append(profile);
    endInsertRows();
}
void ProfileModel::readSettings(QSettings & settings)
{
    settings.beginGroup("ConnectionProfiles");
    int size = settings.beginReadArray("ConnectionProfilesArray");
    for(int i = 0; i< size;++i)
    {
        settings.setArrayIndex(i);
        ConnectionProfile* profile = new ConnectionProfile();
        profile->setAddress(settings.value("address").toString());
        profile->setName( settings.value("name").toString());
        profile->setTitle( settings.value("title").toString());
        profile->setPort(settings.value("port").toInt());
        profile->setServerMode(settings.value("server").toBool());
        profile->setGm(settings.value("gm").toBool());
        QColor color = settings.value("PlayerColor").value<QColor>();
        Player* player = new Player(profile->getName(),color,profile->isGM());
        player->setUserVersion(m_version);
        profile->setPlayer(player);

        Character* character = new Character();
        character->setName(settings.value("CharacterName").toString());
        character->setAvatar(settings.value("CharacterPix").value<QImage>());
        character->setColor(settings.value("CharacterColor").value<QColor>());
        character->setNpc(false);
        profile->setCharacter(character);


        beginInsertRows(QModelIndex(),m_connectionProfileList.size(),m_connectionProfileList.size());
        m_connectionProfileList.append(profile);
        endInsertRows();
    }
    settings.endArray();
    settings.endGroup();



    if(size==0)
    {
        ConnectionProfile* profile = new ConnectionProfile();
        profile->setName(tr("New Player"));
        profile->setTitle(tr("Default"));
        profile->setPort(6660);
        profile->setServerMode(true);
        QColor color = Qt::black;
        Player* player = new Player(profile->getName(),color,profile->isGM());
        player->setUserVersion(m_version);
        profile->setPlayer(player);

        Character* character = new Character(QStringLiteral("Unknown"),Qt::black,false);
        profile->setCharacter(character);
        beginInsertRows(QModelIndex(),m_connectionProfileList.size(),m_connectionProfileList.size());
        m_connectionProfileList.append(profile);
        endInsertRows();
    }
}
ConnectionProfile* ProfileModel::getProfile(const QModelIndex& index)
{
    return getProfile(index.row());
}

int ProfileModel::indexOf(ConnectionProfile *tmp)
{
    return m_connectionProfileList.indexOf(tmp);
}
ConnectionProfile* ProfileModel::getProfile(int index)
{
    if((!m_connectionProfileList.isEmpty())&&(m_connectionProfileList.size()>index))
    {
        return m_connectionProfileList.at(index);
    }
    return NULL;
}
void ProfileModel::removeProfile(ConnectionProfile* profile)
{
    if(NULL!=profile)
    {
        beginRemoveRows(QModelIndex(),m_connectionProfileList.indexOf(profile),m_connectionProfileList.indexOf(profile));
        m_connectionProfileList.removeOne(profile);
        delete profile;
        endRemoveRows();
    }
}

void ProfileModel::writeSettings(QSettings & settings)
{
    settings.beginGroup("ConnectionProfiles");
    settings.beginWriteArray("ConnectionProfilesArray",m_connectionProfileList.size());
    for(int i = 0; i< m_connectionProfileList.size();++i)
    {
        settings.setArrayIndex(i);
        ConnectionProfile* profile = m_connectionProfileList.at(i);
        Player* player = profile->getPlayer();
        Character* character = profile->getCharacter();

        settings.setValue("address",profile->getAddress());
        settings.setValue("name",profile->getName());
        settings.setValue("title",profile->getTitle());
        settings.setValue("server",profile->isServer());
        settings.setValue("port",profile->getPort());
        settings.setValue("gm",profile->isGM());
        settings.setValue("PlayerColor",player->getColor());

        settings.setValue("CharacterColor",character->getColor());
        QImage img = character->getAvatar();
        QVariant var;
        var.setValue(img);
        settings.setValue("CharacterPix",var);
        //settings.setValue("CharacterPath",profile->getPathImg());
        settings.setValue("CharacterName",character->getName());

    }
    settings.endArray();
    settings.endGroup();
}

Qt::ItemFlags ProfileModel::flags(const QModelIndex & index)
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

/////////////////////////////////
///
///SelectConnectionProfileDialog
///
////////////////////////////////

SelectConnectionProfileDialog::SelectConnectionProfileDialog(QString version,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectConnectionProfileDialog),m_currentProfile(NULL),m_version(version)
{
    ui->setupUi(this);
    QSettings settings("rolisteam","rolisteam");

    m_model = new ProfileModel(m_version);
    readSettings(settings);
    ui->m_profileList->setModel(m_model);
    ui->m_progressBar->setVisible(false);

    connect(ui->m_profileList,SIGNAL(clicked(QModelIndex)),this,SLOT(setCurrentProfile(QModelIndex)));
    connect(ui->m_profileList,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(connectToIndex(QModelIndex)));

    ui->m_profileList->setCurrentIndex(m_model->index(0, 0));
    setCurrentProfile(ui->m_profileList->currentIndex());


    connect(ui->m_addProfile,SIGNAL(clicked()),m_model,SLOT(appendProfile()));
    connect(ui->m_cancel,SIGNAL(clicked()),this,SLOT(reject()));
    connect(ui->m_connect,SIGNAL(clicked()),this,SLOT(connectTo()));
    connect(ui->m_selectCharaterAvatar,SIGNAL(clicked()),this,SLOT(openImage()));
    connect(ui->m_delProfileAct,SIGNAL(pressed()),this,SLOT(removeProfile()));

    connect(ui->m_addresseLineEdit,SIGNAL(textChanged(QString)),this,SLOT(checkConnection()));
    connect(ui->m_isServerCheckbox,SIGNAL(toggled(bool)),this,SLOT(checkConnection()));
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
    m_currentProfile = m_model->getProfile(index);
    updateGUI();
}
void SelectConnectionProfileDialog::updateGUI()
{
    if(NULL!=m_currentProfile)
    {
        ui->m_addresseLineEdit->setText(m_currentProfile->getAddress());
        ui->m_name->setText(m_currentProfile->getName());
        ui->m_profileTitle->setText(m_currentProfile->getTitle());
        ui->m_port->setValue(m_currentProfile->getPort());
        ui->m_isServerCheckbox->setChecked(m_currentProfile->isServer());
        ui->m_isGmCheckbox->setChecked(m_currentProfile->isGM());
        ui->m_colorBtn->setColor(m_currentProfile->getPlayer()->getColor());

        if(NULL!=m_currentProfile->getCharacter())
        {
        //character
            ui->m_characterName->setText(m_currentProfile->getCharacter()->getName());
            ui->m_characterColor->setColor(m_currentProfile->getCharacter()->getColor());
            ui->m_selectCharaterAvatar->setIcon(QIcon(QPixmap::fromImage(m_currentProfile->getCharacter()->getAvatar())));
        }
    }
}
void SelectConnectionProfileDialog::removeProfile()
{
  if(NULL!=m_currentProfile)
  {
      if(QMessageBox::No == QMessageBox::question(this,tr("Remove Current Profile"),tr("Do you really want to remove %1 from your connection list ?").arg(m_currentProfile->getTitle())))
      {
          return;
      }
      int i = m_model->indexOf(m_currentProfile);
      m_model->removeProfile(m_currentProfile);
      int size = m_model->rowCount(QModelIndex());
      if(size>i)
      {
          m_currentProfile = m_model->getProfile(i);
      }
      else if(size>0)
      {
          m_currentProfile = m_model->getProfile(size-1);
      }
      else
      {
        m_currentProfile = NULL;
      }
      updateGUI();
  }
}

void SelectConnectionProfileDialog::updateProfile()
{
    ui->m_errorNotification->setStyleSheet("");
    if(NULL!=m_currentProfile)
    {
        m_currentProfile->setAddress(ui->m_addresseLineEdit->text());
        m_currentProfile->setName(ui->m_name->text());
        m_currentProfile->setPort(ui->m_port->value());
        m_currentProfile->setServerMode(ui->m_isServerCheckbox->isChecked());
        m_currentProfile->setTitle(ui->m_profileTitle->text());
        m_currentProfile->setGm(ui->m_isGmCheckbox->isChecked());
        Person* person = m_currentProfile->getPlayer();
        person->setColor(ui->m_colorBtn->color());
        person->setName(ui->m_name->text());

        Character* character = m_currentProfile->getCharacter();
        if(!m_avatarUri.isEmpty())
        {
            character->setAvatar(QImage(m_avatarUri));
        }
        character->setName(ui->m_characterName->text());
        character->setColor(ui->m_characterColor->color());

    }
}
void SelectConnectionProfileDialog::readSettings(QSettings & settings)
{
    m_model->readSettings(settings);
}

void SelectConnectionProfileDialog::writeSettings(QSettings & settings)
{
    m_model->writeSettings(settings);
}

void SelectConnectionProfileDialog::connectToIndex(QModelIndex index)
{
    m_currentProfile = m_model->getProfile(index);
    updateGUI();    
    connectTo();
}
void SelectConnectionProfileDialog::connectTo()
{
    updateProfile();
    ui->m_progressBar->setVisible(true);
    emit tryConnection();
}
void SelectConnectionProfileDialog::openImage()
{
    m_avatarUri = QFileDialog::getOpenFileName(this,tr("Load Avatar"));

    updateProfile();
    //m_currentProfile->getCharacter()->setAvatar(QImage(m_avatarUri));
    updateGUI();
}
void SelectConnectionProfileDialog::checkConnection()
{
    bool valid=false;
    if((!ui->m_addresseLineEdit->text().isEmpty())&&(!ui->m_isServerCheckbox->isChecked()))
    {
        valid=true;
    }
    else if(ui->m_isServerCheckbox->isChecked())
    {
        valid=true;
    }
    ui->m_connect->setEnabled(valid);
}
void SelectConnectionProfileDialog::errorOccurs(QString str)
{
    ui->m_errorNotification->setStyleSheet("font: 19pt ;\nbackground: rgb(255, 0, 0);\ncolor: rgb(0,0,0);");
    ui->m_errorNotification->setText(str);
    ui->m_progressBar->setVisible(false);
}
