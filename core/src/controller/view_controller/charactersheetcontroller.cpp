#include "charactersheetcontroller.h"

#include "charactersheet.h"
#include "charactersheet/charactersheetmodel.h"
#include "charactersheet/imagemodel.h"
#include "charactersheetitem.h"
#include "data/cleveruri.h"
#include "model/charactermodel.h"
#include "worker/modelhelper.h"

CharacterSheetController::CharacterSheetController(CharacterModel* characterModel, CleverURI* uri, QObject* parent)
    : AbstractMediaContainerController(uri, parent)
    , m_model(new CharacterSheetModel)
    , m_imageModel(new ImageModel())
    , m_characterModel(characterModel)
{
    ModelHelper::loadCharacterSheet(uri->getUri(), m_model.get(), m_imageModel.get(), m_rootJson, m_qmlCode);

    for(int j= 0; j < m_model->getCharacterSheetCount(); ++j)
    {
        CharacterSheet* sheet= m_model->getCharacterSheet(j);
        if(nullptr != sheet)
        {
            connect(sheet, &CharacterSheet::updateField, this, &CharacterSheetController::updateFieldFrom);
        }
    }
}

CharacterSheetController::~CharacterSheetController() {}

void CharacterSheetController::saveData() const {}

void CharacterSheetController::loadData() const {}

QAbstractItemModel* CharacterSheetController::model() const
{
    return m_model.get();
}

QAbstractItemModel* CharacterSheetController::imageModel() const
{
    return m_imageModel.get();
}

QAbstractItemModel* CharacterSheetController::characterModel() const
{
    return m_characterModel;
}

QString CharacterSheetController::qmlCode() const
{
    return m_qmlCode;
}

bool CharacterSheetController::cornerEnabled() const
{
    return false;
}

void CharacterSheetController::updateFieldFrom(CharacterSheet* sheet, CharacterSheetItem* item,
                                               const QString& parentPath)
{
    /*   if(nullptr != sheet)
       {
           Player* person= nullptr;
           if(m_localIsGM)
           {
               person= m_sheetToPerson.value(sheet);
           }
           else
           {
               // person= PlayerModel::instance()->getGM();
           }
           if(nullptr != person)
           {
               NetworkMessageWriter msg(NetMsg::CharacterCategory, NetMsg::updateFieldCharacterSheet);
               QStringList recipiants;
               recipiants << person->getUuid();
               msg.setRecipientList(recipiants, NetworkMessage::OneOrMany);
               msg.string8(m_mediaId);
               msg.string8(sheet->getUuid());
               msg.string32(parentPath);
               QJsonObject object;
               item->saveDataItem(object);
               QJsonDocument doc;
               doc.setObject(object);
               msg.byteArray32(doc.toBinaryData());
               msg.sendToServer();
           }
       }*/
}
