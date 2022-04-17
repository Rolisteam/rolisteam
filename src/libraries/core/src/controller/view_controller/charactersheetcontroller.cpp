#include "controller/view_controller/charactersheetcontroller.h"

//#include "charactersheet.h"
#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetitem.h"
#include "data/character.h"

#include "data/player.h"
#include "updater/media/charactersheetupdater.h"
#include "worker/messagehelper.h"
#include "worker/modelhelper.h"

QPointer<CharacterModel> CharacterSheetController::m_characterModel;

CharacterSheetController::CharacterSheetController(const QString& id, const QUrl& path, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::CHARACTERSHEET, parent)
    , m_model(new CharacterSheetModel)
    , m_imageModel(new charactersheet::ImageModel())
    , m_characterSheetUpdater(new CharacterSheetUpdater(id))
{
    setUrl(path);
    ModelHelper::loadCharacterSheet(path.toLocalFile(), m_model.get(), m_imageModel.get(), m_rootJson, m_qmlCode);

    connect(this, &CharacterSheetController::uuidChanged, m_characterSheetUpdater.get(),
            &CharacterSheetUpdater::setMediaId);

    connect(m_model.get(), &CharacterSheetModel::characterSheetHasBeenAdded, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::dataCharacterChange, this, [this] { setModified(); });

    connect(m_imageModel.get(), &charactersheet::ImageModel::internalDataChanged, this, [this] { setModified(); });
}

CharacterSheetController::~CharacterSheetController() {}

void CharacterSheetController::setQmlCode(const QString& qml)
{
    m_qmlCode= qml;
}

void CharacterSheetController::addCharacterSheet(const QJsonObject& data, const QString& charId)
{ // called to create qml page from network
    if(!m_characterModel)
        return;
    auto sheet= new CharacterSheet();
    sheet->load(data);
    auto character= m_characterModel->character(charId);

    auto player= dynamic_cast<Player*>(character->parentPerson());
    if(nullptr == player)
    {
        delete sheet;
        return;
    }
    character->setSheet(sheet);
    m_model->addCharacterSheet(sheet, m_model->getCharacterSheetCount() - 1);
    m_characterSheetUpdater->addCharacterSheetUpdate(sheet, CharacterSheetUpdater::UpdateMode::ONE,
                                                     QStringList() << m_gameMasterId); // GM identity

    emit sheetCreated(sheet, character);
}

void CharacterSheetController::setGameMasterId(const QString& id)
{
    if(id == m_gameMasterId)
        return;
    m_gameMasterId= id;
    emit gameMasterIdChanged();
}

CharacterSheetModel* CharacterSheetController::model() const
{
    return m_model.get();
}

charactersheet::ImageModel* CharacterSheetController::imageModel() const
{
    return m_imageModel.get();
}

CharacterModel* CharacterSheetController::characterModel() const
{
    return m_characterModel;
}

const QJsonObject& CharacterSheetController::rootObject() const
{
    return m_rootJson;
}

QString CharacterSheetController::qmlCode() const
{
    return m_qmlCode;
}

QString CharacterSheetController::gameMasterId() const
{
    return m_gameMasterId;
}

bool CharacterSheetController::cornerEnabled() const
{
    return false;
}

void CharacterSheetController::updateFieldFrom(const QString& sheetId, const QJsonObject& obj,
                                               const QString& parentPath)
{
    auto sheet= m_model->getCharacterSheetById(sheetId);

    if(nullptr == sheet)
        return;

    sheet->setFieldData(obj, parentPath);
}

void CharacterSheetController::shareCharacterSheetToAll(int idx)
{
    auto sheet= m_model->getCharacterSheet(idx);

    if(sheet == nullptr)
        return;

    m_characterSheetUpdater->addCharacterSheetUpdate(sheet, CharacterSheetUpdater::UpdateMode::ALL, QStringList());
}

void CharacterSheetController::shareCharacterSheetTo(const QString& uuid, int idx)
{
    if(!m_characterModel)
        return;
    auto character= m_characterModel->character(uuid);

    auto sheet= m_model->getCharacterSheet(idx);

    if(character == nullptr || sheet == nullptr)
        return;

    // MessageHelper::stopSharingSheet(sheet); TODO Stop sharing
    auto player= dynamic_cast<Player*>(character->parentPerson());

    if(nullptr == player)
        return;
    auto dest= player->uuid();

    sheet->setName(character->name());
    character->setSheet(sheet);
    emit sheetCreated(sheet, character);

    if(localGM())
    {
        MessageHelper::shareCharacterSheet(sheet, character, this);
        m_characterSheetUpdater->addCharacterSheetUpdate(sheet, CharacterSheetUpdater::UpdateMode::ONE,
                                                         QStringList() << dest);
    }
}

void CharacterSheetController::setCharacterModel(CharacterModel* model)
{
    m_characterModel= model;
}
