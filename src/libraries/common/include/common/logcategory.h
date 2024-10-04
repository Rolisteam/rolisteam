#ifndef LOGCATEGORY_H
#define LOGCATEGORY_H

#include <QLoggingCategory>

namespace logger
{

    constexpr auto server{"Server"};
    constexpr auto qmlEngine{"QmlEngine"};
    constexpr auto network{"Network"};
    constexpr auto messaging{"Messaging"};
    constexpr auto imageCtrl{"ImageController"};
    constexpr auto map{"Map"};
    constexpr auto mindMap{"Mindmap"};
    constexpr auto characterSheet{"CharacterSheet"};
    constexpr auto pdf{"Pdf"};
    constexpr auto serialization{"Serialization"};
    constexpr auto campaign{"Campaign"};
    constexpr auto usability{"Usability"};
    constexpr auto rcse{"rcse"};
    constexpr auto rolisteam{"rolisteam"};
    constexpr auto dice{"dice"};
}

static Q_LOGGING_CATEGORY(ServerLogCat, logger::server)
static Q_LOGGING_CATEGORY(QmlEngineCat, logger::qmlEngine)
static Q_LOGGING_CATEGORY(NetworkCat, logger::network)
static Q_LOGGING_CATEGORY(MessagingCat, logger::messaging)
static Q_LOGGING_CATEGORY(ImageCtrlCat, logger::imageCtrl)
static Q_LOGGING_CATEGORY(MapCat, logger::map)
static Q_LOGGING_CATEGORY(MindMapCat, logger::mindMap)
static Q_LOGGING_CATEGORY(CharacterSheetCat, logger::characterSheet)
static Q_LOGGING_CATEGORY(PdfCat, logger::pdf)
static Q_LOGGING_CATEGORY(SerializationCat, logger::serialization)
static Q_LOGGING_CATEGORY(CampaignCat, logger::campaign)
static Q_LOGGING_CATEGORY(UsabilityCat, logger::usability)
static Q_LOGGING_CATEGORY(RcseCat, logger::rcse)
static Q_LOGGING_CATEGORY(RolisteamCat, logger::rolisteam)
static Q_LOGGING_CATEGORY(DiceCat, logger::dice)



#endif // LOGCATEGORY_H
