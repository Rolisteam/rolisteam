#include "controller/view_controller/charactersheetcontroller.h"

// #include "charactersheet.h"
#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetitem.h"
#include "data/character.h"

#include "data/player.h"

QPointer<CharacterModel> CharacterSheetController::m_characterModel;

CharacterSheetController::CharacterSheetController(const QString& id, const QUrl& path, QObject* parent)
    : MediaControllerBase(id, Core::ContentType::CHARACTERSHEET, parent)
    , m_model(new CharacterSheetModel)
    , m_imageModel(new charactersheet::ImageModel())
{
    setUrl(path);

    connect(m_model.get(), &CharacterSheetModel::characterSheetHasBeenAdded, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::rowsInserted, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::rowsRemoved, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::modelReset, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::dataChanged, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::columnsInserted, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::columnsRemoved, this, [this] { setModified(); });
    connect(m_model.get(), &CharacterSheetModel::dataCharacterChange, this, [this] { setModified(); });

    connect(m_imageModel.get(), &charactersheet::ImageModel::internalDataChanged, this, [this] { setModified(); });
    connect(m_imageModel.get(), &charactersheet::ImageModel::rowsInserted, this, [this] { setModified(); });
    connect(m_imageModel.get(), &charactersheet::ImageModel::rowsRemoved, this, [this] { setModified(); });
    connect(m_imageModel.get(), &charactersheet::ImageModel::modelReset, this, [this] { setModified(); });
    connect(m_imageModel.get(), &charactersheet::ImageModel::dataChanged, this, [this] { setModified(); });

    connect(this, &CharacterSheetController::share, this,
            [this](CharacterSheetController* ctrl, CharacterSheet* sheet, CharacterSheetUpdater::SharingMode mode,
                   Character* character, const QStringList&)
            {
                Q_UNUSED(ctrl)
                m_sheetData.append({sheet->uuid(), character ? character->uuid() : QString{},
                                    mode == CharacterSheetUpdater::SharingMode::ALL, false});
                setModified();
            });
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
    if(!charId.isEmpty())
    {
        if(!character)
            return;

        auto player= dynamic_cast<Player*>(character->parentPerson());
        if(nullptr == player)
        {
            delete sheet;
            qWarning() << "No player found, the sheet cannot be added";
            return;
        }
        character->setSheet(sheet);
    }
    m_model->addCharacterSheet(sheet, m_model->getCharacterSheetCount());
    m_sheetData.append({sheet->uuid(), charId, false, true});
    setModified();
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

const QJsonObject& CharacterSheetController::rootJson() const
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
    if(!localGM())
        return;

    auto sheet= m_model->getCharacterSheet(idx);

    if(sheet == nullptr)
        return;

    emit share(this, sheet, CharacterSheetUpdater::SharingMode::ALL, nullptr, {});
    emit sheetCreated(sheet, nullptr);
}

void CharacterSheetController::stopSharing(const QString& uuid)
{
    qDebug() << "Charactershsett controller: stopSharing" << uuid;
    auto it= std::find_if(std::begin(m_sheetData), std::end(m_sheetData),
                          [uuid](const CharacterSheetInfo& info) { return info.m_sheetId == uuid; });

    if(it == std::end(m_sheetData))
        return;

    emit removedSheet(it->m_sheetId, this->uuid(), it->m_characterId);

    m_sheetData.erase(it);
    setModified();
}

void CharacterSheetController::shareCharacterSheetTo(const CharacterSheetInfo& info)
{
    if(!m_characterModel)
        return;
    auto character= m_characterModel->character(info.m_characterId);
    auto sheet= m_model->getCharacterSheetById(info.m_sheetId);

    if(!info.everyone)
    {
        shareCharacterSheetTo(character, sheet);
    }
    else
    {
        auto sheet= m_model->getCharacterSheetById(info.m_sheetId);
        emit share(this, sheet, CharacterSheetUpdater::SharingMode::ALL, nullptr, {});
    }
}

void CharacterSheetController::shareCharacterSheetTo(const QString& uuid, int idx)
{
    if(!m_characterModel)
        return;
    auto character= m_characterModel->character(uuid);
    auto sheet= m_model->getCharacterSheet(idx);

    shareCharacterSheetTo(character, sheet);
}

void CharacterSheetController::shareCharacterSheetTo(Character* character, CharacterSheet* sheet)
{
    if(character == nullptr || sheet == nullptr)
        return;

    auto player= dynamic_cast<Player*>(character->parentPerson());

    if(nullptr == player)
        return;
    auto dest= player->uuid();

    sheet->setName(character->name());
    character->setSheet(sheet);
    emit sheetCreated(sheet, character);

    if(localGM())
    {
        emit share(this, sheet, CharacterSheetUpdater::SharingMode::ONE, character, {dest});
    }
}

void CharacterSheetController::setCharacterModel(CharacterModel* model)
{
    m_characterModel= model;
}

void CharacterSheetController::setRootJson(const QJsonObject& newRootJson)
{
    if(m_rootJson == newRootJson)
        return;
    m_rootJson= newRootJson;
    emit rootJsonChanged();
}

const QList<CharacterSheetInfo>& CharacterSheetController::sheetData() const
{
    return m_sheetData;
}

void CharacterSheetController::addSheetData(const CharacterSheetInfo& info)
{
    m_sheetData.append(info);
}

bool CharacterSheetController::alreadySharing(const QString& characterId, const QString& charactersheetId) const
{
    auto const& it= std::find_if(std::begin(m_sheetData), std::end(m_sheetData),
                                 [characterId, charactersheetId](const CharacterSheetInfo& data)
                                 { return (data.m_characterId == characterId && data.m_sheetId == charactersheetId); });

    return it != std::end(m_sheetData);
}

bool CharacterSheetController::alreadySharing(const QString& charactersheetId) const
{
    auto const& it= std::find_if(std::begin(m_sheetData), std::end(m_sheetData),
                                 [charactersheetId](const CharacterSheetInfo& data)
                                 { return (data.m_characterId == charactersheetId); });

    return it != std::end(m_sheetData);
}

QString CharacterSheetController::characterSheetIdFromIndex(int i) const
{
    qDebug() << "CharacterSheetId index:" << i;
    auto const& sheet= m_model->getCharacterSheet(i);
    if(!sheet)
        return {};
    return sheet->uuid();
}

bool CharacterSheetController::hasCharacterSheet(const QString& id) const
{
    return (nullptr != m_model->getCharacterSheetById(id));
}

CharacterSheet* CharacterSheetController::characterSheetFromId(const QString& id) const
{
    return m_model->getCharacterSheetById(id);
}

int CharacterSheetController::characterCount() const
{
    return m_characterModel->rowCount();
}

void CharacterSheetController::merge(CharacterSheetController* otherCtrl)
{
    auto omodel= otherCtrl->model();

    for(int i= 0; i < omodel->getCharacterSheetCount(); ++i)
    {
        auto sheet= omodel->getCharacterSheet(i);
        omodel->releaseCharacterSheet(sheet);
        m_model->addCharacterSheet(sheet);
    }
    otherCtrl->deleteLater();
    setModified();
}
