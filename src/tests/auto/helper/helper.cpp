#include "helper.h"
#include "charactersheet/charactersheet.h"
#include "charactersheet_widgets/sheetwidget.h"
#include "controller/view_controller/selectconnprofilecontroller.h"
#include "controller/view_controller/sharednotecontroller.h"
#include "controller/view_controller/webpagecontroller.h"
#include "data/charactervision.h"
#include "media/mediatype.h"
#include "mindmap/data/linkcontroller.h"
#include "mindmap/data/mindnode.h"
#include "model/filteredcharactermodel.h"
#include "model/participantmodel.h"
#include "network/networkmessagewriter.h"
#include "network/serverconnection.h"
#include "test_root_path.h"
#include "utils/iohelper.h"

#include <QBrush>
#include <QColor>
#include <QCursor>
#include <QDate>
#include <QFont>
#include <QFontDatabase>
#include <QIcon>
#include <QLine>
#include <QMetaObject>
#include <QMetaProperty>
#include <QMetaType>
#include <QPainterPath>
#include <QPixmap>
#include <QPolygon>
#include <QPolygonF>
#include <QRect>
#include <QRectF>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTest>
#include <QUuid>
#include <QVariant>
#include <QtHttpServer>
#include <limits>

namespace Helper
{
QVariantList values(QMetaType type, QVariant currentValue, QObject* parent= nullptr)
{
    QVariantList res;
    switch(type.id())
    {
    case QMetaType::User:
    case QMetaType::UnknownType:
    case QMetaType::Void:
    case QMetaType::Nullptr:
    case QMetaType::VoidStar:
    case QMetaType::QObjectStar:
    case QMetaType::QVariantPair:
    case QMetaType::QVariant:
    case QMetaType::QVariantList:
    case QMetaType::QColorSpace:
    case QMetaType::QModelIndex:
    case QMetaType::QPersistentModelIndex:
    case QMetaType::QTextLength:
    case QMetaType::QVariantMap:
    case QMetaType::QVariantHash:
    case QMetaType::QTextFormat:
    case QMetaType::QRegion:
    case QMetaType::QBitArray:
    case QMetaType::QPalette:
    case QMetaType::QKeySequence:
    case QMetaType::QSizePolicy:
        break;
    case QMetaType::Char16:
        res= {static_cast<char16_t>('A'), static_cast<char16_t>('1'), static_cast<char16_t>('z'),
              std::numeric_limits<char16_t>::max(), std::numeric_limits<char16_t>::lowest()};
        break;
    case QMetaType::Char32:
        res= {static_cast<char32_t>('A'), static_cast<char32_t>('1'), static_cast<char32_t>('z'),
              std::numeric_limits<char32_t>::max(), std::numeric_limits<char32_t>::lowest()};
        break;
    case QMetaType::Bool:
        res= {true, false};
        break;
    case QMetaType::Int:
        res= {std::numeric_limits<int>::max(), std::numeric_limits<int>::lowest()};
        break;
    case QMetaType::UInt:
        res= {std::numeric_limits<uint>::max(), std::numeric_limits<uint>::lowest()};
        break;
    case QMetaType::Double:
        res= {std::numeric_limits<double>::max(), std::numeric_limits<double>::lowest()};
        break;
    case QMetaType::QChar:
        res= {QChar('A'), QChar('1'), QChar('z')};
        break;
    case QMetaType::QString:
        res= {QString("aaaa"), QString("zzzz"), QString("ppppp")};
        break;
    case QMetaType::QByteArray:
        res= {QByteArray(), QByteArray("aaaa"), QByteArray("zzzz")};
        break;
    case QMetaType::Long:
        res= {QVariant::fromValue(std::numeric_limits<long>::max()),
              QVariant::fromValue(std::numeric_limits<long>::min())};
        break;
    case QMetaType::LongLong:
        res= {std::numeric_limits<long long>::max(), std::numeric_limits<long long>::min()};
        break;
    case QMetaType::Short:
        res= {std::numeric_limits<short>::max(), std::numeric_limits<short>::min()};
        break;
    case QMetaType::Char:
        res= {static_cast<char>('A'), static_cast<char>('1'), static_cast<char>('z'), std::numeric_limits<char>::max(),
              std::numeric_limits<char>::min()};
        break;
    case QMetaType::ULong:
        res= {QVariant::fromValue(std::numeric_limits<unsigned long>::max()),
              QVariant::fromValue(std::numeric_limits<unsigned long>::min())};
        break;
    case QMetaType::ULongLong:
        res= {std::numeric_limits<unsigned long long>::max(), std::numeric_limits<unsigned long long>::min()};
        break;
    case QMetaType::UShort:
        res= {std::numeric_limits<unsigned short>::max(), std::numeric_limits<unsigned short>::min()};
        break;
    case QMetaType::SChar:
        res= {static_cast<signed char>('A'), static_cast<signed char>('1'), static_cast<signed char>('z'),
              std::numeric_limits<signed char>::max(), std::numeric_limits<signed char>::min()};
        break;
    case QMetaType::UChar:
        res= {static_cast<unsigned char>('A'), static_cast<unsigned char>('1'), static_cast<unsigned char>('z'),
              std::numeric_limits<unsigned char>::max(), std::numeric_limits<unsigned char>::min()};
        break;
    case QMetaType::Float:
        res= {std::numeric_limits<float>::max(), std::numeric_limits<float>::min()};
        break;
    case QMetaType::QCursor:
        res= {QVariant::fromValue(QCursor(Qt::ArrowCursor)), QVariant::fromValue(QCursor(Qt::SizeHorCursor))};
        break;
    case QMetaType::QDate:
        res= {QDate::currentDate(), QDate()};
        break;
    case QMetaType::QSize:
        res= {QSize(10, 10), QSize(100, 100), QSize(1, 1), QSize()};
        break;
    case QMetaType::QTime:
        res= {QTime(10, 10), QTime(1, 1), QTime(23, 34), QTime::currentTime()};
        break;
    case QMetaType::QPolygon:
        res= {QPolygon({QPoint{0, 0}, QPoint{10, 0}, QPoint{10, 10}, QPoint{0, 10}}),
              QPolygon({QPoint{0, 0}, QPoint{100, 0}, QPoint{100, 100}, QPoint{0, 100}})};
        break;
    case QMetaType::QPolygonF:
        res= {QPolygonF({QPointF{0., 0.}, QPointF{10., 0.}, QPointF{10., 10.}, QPointF{0., 10.}}),
              QPolygonF({QPointF{0., 0.}, QPointF{100., 0.}, QPointF{100., 100.}, QPointF{0., 100.}})};
        break;
    case QMetaType::QColor:
        res= {QColor(Qt::black), QColor(Qt::red), QColor(Qt::green), QColor(Qt::darkCyan)};
        break;
    case QMetaType::QSizeF:
        res= {QSizeF(10., 10.), QSizeF(100., 100.), QSizeF(1., 1.), QSizeF()};
        break;
    case QMetaType::QRectF:
        res= {QRectF(10., 10., 10., 10.), QRectF(100., 100., 100., 100.), QRectF(1., 1., 1., 1.),
              QRectF(12., 12., 12., 12.)};
        break;
    case QMetaType::QLine:
        res= {QLine(10, 10, 20, 20), QLine(100, 100, 200, 200), QLine()};
        break;
    case QMetaType::QStringList:
        res= {QStringList{"aaa", "bbbb", "cccc"}, QStringList{"zzz", "yyyyy", "xxxxx"}};
        break;
    case QMetaType::QIcon:
        res= {QIcon(":/img/arbre_square_500.jpg"), QIcon(":/img/lion.jpg"), QIcon(":/img/white.png")};
        break;
    case QMetaType::QPen:
        res= {QStringList{"aaa", "bbbb", "cccc"}, QStringList{"zzz", "yyyyy", "xxxxx"}};
        break;
    case QMetaType::QLineF:
        res= {QLineF(10., 10., 20., 20.), QLineF(100., 100., 200., 200.), QLineF()};
        break;
    case QMetaType::QRect:
        res= {QRect(10, 10, 10, 10), QRect(100, 100, 100, 100), QRect(1, 1, 1, 1), QRect()};
        break;
    case QMetaType::QPoint:
        res= {QPoint(0, 0), QPoint{10, 10}, QPoint{100, 100}, QPoint{50, 500}};
        break;
    case QMetaType::QUrl:
        res= {QUrl{"https://www.rolisteam.org"}, QUrl{"https://doc.rolisteam.org"}, QUrl{"https://google.com"},
              QUrl{"https://fr.wikipedia.org"}, QUrl{"qrc:/img/lion.jpg"}};
        break;
    case QMetaType::QRegularExpression:
        res= {QRegularExpression{"(.*)k(.*)"}, QRegularExpression{"([0-9]+)C([0-9]+)"}};
        break;
    case QMetaType::QDateTime:
        res= {QDateTime::currentDateTime(), QDateTime(QDate(2012, 7, 6), QTime(8, 30, 0))};
        break;
    case QMetaType::QPointF:
        res= {QPointF(0., 0.), QPointF{10., 10.}, QPointF{100., 100.}, QPointF{50., 500.}};
        break;
    case QMetaType::QFont:
    {
        auto italic= QFontDatabase::systemFont(QFontDatabase::GeneralFont);
        italic.setItalic(true);

        auto bold= QFontDatabase::systemFont(QFontDatabase::TitleFont);
        bold.setBold(true);

        res= {QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont),
              QFontDatabase::systemFont(QFontDatabase::FixedFont), italic, bold};
    }
    break;
    case QMetaType::QBrush:
        res= {QBrush{Qt::red}, QBrush{Qt::black}, QBrush{Qt::green}};
        break;
    case QMetaType::QImage:
        res= {QImage(":/img/arbre_square_500.jpg"), QImage(":/img/lion.jpg"), QImage(":/img/white.png")};
        break;
    case QMetaType::QPixmap:
        res= {QPixmap(":/img/arbre_square_500.jpg"), QPixmap(":/img/lion.jpg"), QPixmap(":/img/white.png")};
        break;
    case QMetaType::QLocale:
        break;
    case QMetaType::QBitmap:
        break;
    case QMetaType::QTransform:
        break;
    case QMetaType::QMatrix4x4:
        break;
    case QMetaType::QVector2D:
        break;
    case QMetaType::QVector3D:
        break;
    case QMetaType::QVector4D:
        break;
    case QMetaType::QQuaternion:
        break;
    case QMetaType::QEasingCurve:
        break;
    case QMetaType::QJsonValue:
        break;
    case QMetaType::QJsonObject:
        res= {QJsonObject({{"key1", "value1"}, {"key2", "value2"}}),
              QJsonObject({{"keyA", "valueA"}, {"keyB", "valueB"}})};
        break;
    case QMetaType::QJsonArray:
        break;
    case QMetaType::QJsonDocument:
        break;
    case QMetaType::QCborValue:
        break;
    case QMetaType::QCborArray:
        break;
    case QMetaType::QCborMap:
        break;
    case QMetaType::QCborSimpleType:
        break;
    case QMetaType::QUuid:
        res= {QUuid().createUuid(), QUuid().createUuid()};
        break;
    case QMetaType::QByteArrayList:
        break;
    }

    if(res.empty())
    {
        if(qMetaTypeId<CharacterSheet*>() == type.id())
        {
            res= {QVariant::fromValue(new CharacterSheet(parent)), QVariant::fromValue(new CharacterSheet(parent))};
        }
        else if(qMetaTypeId<Core::ScaleUnit>() == type.id())
        {
            res= {Core::ScaleUnit::M,    Core::ScaleUnit::KM,   Core::ScaleUnit::CM,   Core::ScaleUnit::MILE,
                  Core::ScaleUnit::YARD, Core::ScaleUnit::INCH, Core::ScaleUnit::FEET, Core::ScaleUnit::PX};
        }
        else if(qMetaTypeId<Core::PermissionMode>() == type.id())
        {
            res= {Core::PermissionMode::GM_ONLY, Core::PermissionMode::PC_MOVE, Core::PermissionMode::PC_ALL};
        }
        else if(qMetaTypeId<Core::GridPattern>() == type.id())
        {
            res= {QVariant::fromValue(Core::GridPattern::HEXAGON), QVariant::fromValue(Core::GridPattern::SQUARE),
                  QVariant::fromValue(Core::GridPattern::NONE), QVariant::fromValue(Core::GridPattern::OCTOGON)};
        }
        else if(qMetaTypeId<Core::VisibilityMode>() == type.id())
        {
            res= {QVariant::fromValue(Core::VisibilityMode::NONE), QVariant::fromValue(Core::VisibilityMode::FOGOFWAR),
                  QVariant::fromValue(Core::VisibilityMode::ALL), QVariant::fromValue(Core::VisibilityMode::HIDDEN)};
        }
        else if(qMetaTypeId<Core::Layer>() == type.id())
        {
            res= {QVariant::fromValue(Core::Layer::GROUND),          QVariant::fromValue(Core::Layer::OBJECT),
                  QVariant::fromValue(Core::Layer::CHARACTER_LAYER), QVariant::fromValue(Core::Layer::FOG),
                  QVariant::fromValue(Core::Layer::GRIDLAYER),       QVariant::fromValue(Core::Layer::NONE),
                  QVariant::fromValue(Core::Layer::GAMEMASTER_LAYER)};
        }
        else if(qMetaTypeId<Core::SelectableTool>() == type.id())
        {
            res= {QVariant::fromValue(Core::SelectableTool::PEN),
                  QVariant::fromValue(Core::SelectableTool::EMPTYRECT),
                  QVariant::fromValue(Core::SelectableTool::LINE),
                  QVariant::fromValue(Core::SelectableTool::FILLRECT),
                  QVariant::fromValue(Core::SelectableTool::EMPTYELLIPSE),
                  QVariant::fromValue(Core::SelectableTool::FILLEDELLIPSE),
                  QVariant::fromValue(Core::SelectableTool::TEXT),
                  QVariant::fromValue(Core::SelectableTool::HANDLER),
                  QVariant::fromValue(Core::SelectableTool::NonPlayableCharacter),
                  QVariant::fromValue(Core::SelectableTool::PlayableCharacter),
                  QVariant::fromValue(Core::SelectableTool::IMAGE),
                  QVariant::fromValue(Core::SelectableTool::RULE),
                  QVariant::fromValue(Core::SelectableTool::PATH),
                  QVariant::fromValue(Core::SelectableTool::ANCHOR),
                  QVariant::fromValue(Core::SelectableTool::TEXTBORDER),
                  QVariant::fromValue(Core::SelectableTool::PIPETTE),
                  QVariant::fromValue(Core::SelectableTool::BUCKET),
                  QVariant::fromValue(Core::SelectableTool::HIGHLIGHTER)};
        }
        else if(qMetaTypeId<Core::EditionMode>() == type.id())
        {
            res= {QVariant::fromValue(Core::EditionMode::Painting), QVariant::fromValue(Core::EditionMode::Mask),
                  QVariant::fromValue(Core::EditionMode::Unmask)};
        }
        else if(qMetaTypeId<Core::ContentType>() == type.id())
        {
            res= {QVariant::fromValue(Core::ContentType::VECTORIALMAP),
                  QVariant::fromValue(Core::ContentType::PICTURE),
                  QVariant::fromValue(Core::ContentType::NOTES),
                  QVariant::fromValue(Core::ContentType::CHARACTERSHEET),
                  QVariant::fromValue(Core::ContentType::SHAREDNOTE),
                  QVariant::fromValue(Core::ContentType::PDF),
                  QVariant::fromValue(Core::ContentType::WEBVIEW),
                  QVariant::fromValue(Core::ContentType::INSTANTMESSAGING),
                  QVariant::fromValue(Core::ContentType::MINDMAP),
                  QVariant::fromValue(Core::ContentType::UNKNOWN)};
        }
        else if(qMetaTypeId<Core::State>() == type.id())
        {
            res= {QVariant::fromValue(Core::State::Unloaded), QVariant::fromValue(Core::State::Hidden),
                  QVariant::fromValue(Core::State::Displayed)};
        }
        else if(qMetaTypeId<Core::MediaType>() == type.id())
        {
            res= {QVariant::fromValue(Core::MediaType::Unknown),
                  QVariant::fromValue(Core::MediaType::TokenFile),
                  QVariant::fromValue(Core::MediaType::ImageFile),
                  QVariant::fromValue(Core::MediaType::MapFile),
                  QVariant::fromValue(Core::MediaType::TextFile),
                  QVariant::fromValue(Core::MediaType::CharacterSheetFile),
                  QVariant::fromValue(Core::MediaType::PdfFile),
                  QVariant::fromValue(Core::MediaType::WebpageFile),
                  QVariant::fromValue(Core::MediaType::MindmapFile),
                  QVariant::fromValue(Core::MediaType::AudioFile),
                  QVariant::fromValue(Core::MediaType::PlayListFile)};
        }
        else if(qMetaTypeId<Core::LoadingMode>() == type.id())
        {
            res= {QVariant::fromValue(Core::LoadingMode::Internal), QVariant::fromValue(Core::LoadingMode::Linked)};
        }
        else if(qMetaTypeId<std::vector<QPointF>>() == type.id())
        {
            res= {QVariant::fromValue(std::vector({QPointF(10., 10.), QPointF(100., 100.), QPointF(300., 100.)})),
                  QVariant::fromValue(std::vector({QPointF(500., 10.), QPointF(100., 400.), QPointF(10., 1000.)}))};
        }
        else if(qMetaTypeId<QList<QPointF>>() == type.id())
        {
            res= {QVariant::fromValue(QList({QPointF(10., 10.), QPointF(100., 100.), QPointF(300., 100.)})),
                  QVariant::fromValue(QList({QPointF(500., 10.), QPointF(100., 400.), QPointF(10., 1000.)}))};
        }
        else if(qMetaTypeId<SelectConnProfileController::ConnectionState>() == type.id())
        {
            res= {
                QVariant::fromValue(SelectConnProfileController::ConnectionState::LOADING),
                QVariant::fromValue(SelectConnProfileController::ConnectionState::LOADED),
                QVariant::fromValue(SelectConnProfileController::ConnectionState::CONNECTING),
                QVariant::fromValue(SelectConnProfileController::ConnectionState::IDLE),
            };
        }
        /*else if(qMetaTypeId<ParticipantModel::Permission>() == type.id())
        {
            res= {QVariant::fromValue(ParticipantModel::Permission::readWrite),
                  QVariant::fromValue(ParticipantModel::Permission::readOnly),
                  QVariant::fromValue(ParticipantModel::Permission::hidden)};
        }*/
        else if(qMetaTypeId<SharedNoteController::HighlightedSyntax>() == type.id())
        {
            res= {QVariant::fromValue(SharedNoteController::HighlightedSyntax::None),
                  QVariant::fromValue(SharedNoteController::HighlightedSyntax::MarkDown)};
        }
        else if(qMetaTypeId<WebpageController::State>() == type.id())
        {
            res= {QVariant::fromValue(WebpageController::State::localIsGM),
                  QVariant::fromValue(WebpageController::State::LocalIsPlayer),
                  QVariant::fromValue(WebpageController::State::RemoteView)};
        }
        else if(qMetaTypeId<WebpageController::SharingMode>() == type.id())
        {
            res= {QVariant::fromValue(WebpageController::SharingMode::Url),
                  QVariant::fromValue(WebpageController::SharingMode::None)};
        }
        else if(qMetaTypeId<Qt::CaseSensitivity>() == type.id())
        {
            res= {QVariant::fromValue(Qt::CaseInsensitive), QVariant::fromValue(Qt::CaseSensitive)};
        }
        else if(qMetaTypeId<campaign::FilteredCharacterModel::Definition>() == type.id())
        {
            res= {QVariant::fromValue(campaign::FilteredCharacterModel::Definition::All),
                  QVariant::fromValue(campaign::FilteredCharacterModel::Definition::With),
                  QVariant::fromValue(campaign::FilteredCharacterModel::Definition::Without)};
        }
        else if(qMetaTypeId<campaign::FilteredCharacterModel::HealthState>() == type.id())
        {
            res= {QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_All),
                  QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_Full),
                  QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_Dead),
                  QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_Injured)};
        }
        else if(qMetaTypeId<campaign::FilteredCharacterModel::HealthState>() == type.id())
        {
            res= {QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_All),
                  QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_Full),
                  QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_Dead),
                  QVariant::fromValue(campaign::FilteredCharacterModel::HealthState::HS_Injured)};
        }
        else if(qMetaTypeId<QPainterPath>() == type.id())
        {
            res= {QVariant::fromValue(QPainterPath(QPointF(1200., 500.))),
                  QVariant::fromValue(QPainterPath(QPointF(200., 100.))), QVariant::fromValue(QPainterPath())};
        }
        else if(qMetaTypeId<Person*>() == type.id())
        {
            res= {QVariant::fromValue(new Player()), QVariant::fromValue(new Player())};
        }
        else if(qMetaTypeId<ServerConnection*>() == type.id())
        {
            res= {QVariant::fromValue(new ServerConnection(nullptr)),
                  QVariant::fromValue(new ServerConnection(nullptr))};
        }
        else if(qMetaTypeId<mindmap::PositionedItem*>() == type.id())
        {
            res= {QVariant::fromValue(new mindmap::MindNode()), QVariant::fromValue(new mindmap::MindNode())};
        }
        else if(qMetaTypeId<campaign::NonPlayableCharacter*>() == type.id())
        {
            res= {QVariant::fromValue(new campaign::NonPlayableCharacter()),
                  QVariant::fromValue(new campaign::NonPlayableCharacter())};
        }
        else if(qMetaTypeId<RolisteamTheme*>() == type.id())
        {
            res= {QVariant::fromValue(new RolisteamTheme())};
        }
        else if(qMetaTypeId<campaign::Campaign::State>() == type.id())
        {
            res= {QVariant::fromValue(campaign::Campaign::State::Ready),
                  QVariant::fromValue(campaign::Campaign::State::None),
                  QVariant::fromValue(campaign::Campaign::State::Checking)};
        }
        else if(qMetaTypeId<CharacterVision::SHAPE>() == type.id())
        {
            res= {QVariant::fromValue(CharacterVision::SHAPE::ANGLE),
                  QVariant::fromValue(CharacterVision::SHAPE::DISK)};
        }
        else if(qMetaTypeId<mindmap::LinkController::Direction>() == type.id())
        {
            res= {QVariant::fromValue(mindmap::LinkController::StartToEnd),
                  QVariant::fromValue(mindmap::LinkController::Both),
                  QVariant::fromValue(mindmap::LinkController::EndToStart)};
        }
    }

    if(res.empty())
    {
        qDebug() << "Unmanaged Type::" << type.id() << type.name();
    }

    if(res.removeOne(currentValue))
        res.append(currentValue);

    return res;
}
namespace cv= Core::vmapkeys;
const std::map<QString, QVariant> buildRectController(bool filled, const QRectF& rect, const QPointF& pos)
{
    return {{"filled", filled},
            {"tool", filled ? Core::SelectableTool::FILLRECT : Core::SelectableTool::EMPTYRECT},
            {"rect", rect},
            {"position", pos}};
}

const std::map<QString, QVariant> buildTextController(bool border, const QString& text, const QRectF& rect,
                                                      const QPointF& pos)
{
    return {{"border", border},
            {"rect", rect},
            {"position", pos},
            {"text", text},
            {"tool", border ? Core::SelectableTool::TEXT : Core::SelectableTool::TEXTBORDER}};
}

const std::map<QString, QVariant> buildEllipseController(bool filled, qreal rx, qreal ry, const QPointF& pos)
{
    return {{"filled", filled},
            {"tool", filled ? Core::SelectableTool::FILLEDELLIPSE : Core::SelectableTool::EMPTYELLIPSE},
            {"rx", rx},
            {"ry", ry},
            {"position", pos}};
}

const std::map<QString, QVariant> buildImageController(const QString& path, const QRectF& rect, const QPointF& pos)
{
    return {{cv::KEY_PATH, path}, {"rect", rect}, {"tool", Core::SelectableTool::IMAGE}, {"position", pos}};
}

const std::map<QString, QVariant> buildPenController(bool filled, const std::vector<QPointF>& points,
                                                     const QPointF& pos)
{
    return {{"filled", filled},
            {"tool", Core::SelectableTool::PEN},
            {"points", QVariant::fromValue(points)},
            {"position", pos}};
}

HELPER_EXPORT const std::map<QString, QVariant> buildTokenController(bool isNpc, const QPointF& pos)
{
    return {{"isNpc", isNpc},
            {"tool", isNpc ? Core::SelectableTool::NonPlayableCharacter : Core::SelectableTool::PlayableCharacter},
            {"position", pos}};
}
const std::map<QString, QVariant> buildPathController(bool filled, const std::vector<QPointF>& points,
                                                      const QPointF& pos)
{
    return {{"filled", filled},
            {"tool", Core::SelectableTool::PATH},
            {"points", QVariant::fromValue(points)},
            {"position", pos}};
}
const std::map<QString, QVariant> buildLineController(const QPointF& p1, const QPointF& p2, const QPointF& pos)
{
    return {{"tool", Core::SelectableTool::LINE},
            {"start", QVariant::fromValue(p1)},
            {"end", QVariant::fromValue(p2)},
            {"position", pos}};
}

TestMessageSender::TestMessageSender() {}

void TestMessageSender::sendMessage(const NetworkMessage* msg)
{
    auto const writer= dynamic_cast<const NetworkMessageWriter*>(msg);
    if(writer)
        m_msgData= writer->data();
}

QByteArray TestMessageSender::messageData() const
{
    return m_msgData;
}

std::pair<bool, QStringList> testAllProperties(QObject* obj, QStringList ignoredProperties, bool setAgain)
{
    if(!obj)
        return {};
    auto meta= obj->metaObject();
    bool res= false;
    QStringList unmanaged;
    for(int i= meta->propertyOffset(); i < meta->propertyCount(); ++i)
    {
        auto p= meta->property(i);

        if(ignoredProperties.contains(p.name()))
            continue;

        if(p.isConstant())
        {
            p.read(obj);
            continue;
        }

        if(p.hasNotifySignal() && p.isWritable())
        {

            auto vs= values(p.metaType(), p.read(obj), obj);

            if(vs.isEmpty())
            {
                unmanaged.append(p.name());
                continue;
            }

            auto currentval= p.read(obj);
            QSignalSpy spy(obj, p.notifySignal());
            int countChange= 0;
            for(const auto& v : vs)
            {
                if(!p.write(obj, v))
                    qDebug() << "write failed for " << p.name() << v;
                res= true;

                if(setAgain)
                {
                    p.write(obj, v);
                }
            }

            spy.wait(10);

            if(!QTest::qCompare(spy.count(), vs.size(), "spy.count()", "vs.size()", __FILE__, __LINE__))
            {
                qDebug() << p.name() << "values:" << vs << "current val:" << currentval
                         << "change count:" << countChange;
            }

            p.write(obj, currentval);
        }
    }

    return std::make_pair(res, unmanaged);
}

QString imagePath(bool isSquare)
{
    QStringList array{":/img/girafe.jpg", ":/img/lion.jpg",  ":/img/control_life_bar.gif", ":/img/girafe3.jpg",
                      ":/img/lion3.jpg",  ":/img/white.png", ":/img/arbre_500.jpg"};

    if(isSquare)
        array= QStringList{":/img/arbre_square_500.jpg"};

    return array[generate<int>(0, array.size() - 1)];
}

QUrl imagePathUrl(bool isSquare)
{
    QList<QUrl> array{QUrl("qrc:/img/girafe.jpg"),   QUrl("qrc:/img/lion.jpg"),  QUrl("qrc:/img/control_life_bar.gif"),
                      QUrl("qrc:/img/girafe3.jpg"),  QUrl("qrc:/img/lion3.jpg"), QUrl("qrc:/img/white.png"),
                      QUrl("qrc:/img/arbre_500.jpg")};

    if(isSquare)
        array= QList<QUrl>{QUrl("qrc:/img/arbre_square_500.jpg")};

    return array[generate<int>(0, array.size() - 1)];
}

QByteArray imageData(bool isSquare)
{
    return utils::IOHelper::loadFile(imagePath(isSquare));
}

QByteArray randomData(int length)
{
    QByteArray res;
    res.resize(length);

    std::generate(res.begin(), res.end(),
                  []
                  {
                      static std::random_device rd;
                      static std::mt19937 generator(rd());
                      static std::uniform_int_distribution<int> dist(0, std::numeric_limits<char>::max());
                      return dist(generator);
                  });

    return res;
}

QColor randomColor()
{
    return QColor(generate<int>(0, 255), generate<int>(0, 255), generate<int>(0, 255));
}

QPointF randomPoint()
{
    return QPointF{generate<qreal>(0., 800.), generate<qreal>(0., 800.)};
}

QObject* initWebServer(int port)
{
    auto server= new QHttpServer();

    server->route("/image/<arg>",
                  [](const QUrl& url) {
                      return QHttpServerResponse::fromFile(
                          QStringLiteral("%1/resources/img/%2").arg(tests::root_path, url.path()));
                  });

    server->listen(QHostAddress::Any, port);

    return server;
}

QUrl randomUrl()
{
    static QStringList files{"arbre_500.jpg",        "girafe3.jpg", "lion.jpg",           "Seppun_tashime.jpg",
                             "arbre_square_500.jpg", "girafe.jpg",  "lynn_gray_rike.jpg", "white.png",
                             "control_life_bar.gif", "lion3.jpg",   "predateur.jpg"};
    auto file= files[generate<int>(0, files.size() - 1)];

    return QUrl(QString("http://127.0.0.1:9090/image/%1").arg(file));
}

QRectF randomRect()
{
    return QRectF{generate<qreal>(0., 800.), generate<qreal>(0., 800.), generate<qreal>(0., 800.),
                  generate<qreal>(0., 800.)};
}

QMovie randomMovie()
{
    static QStringList list{"qrc:/img/control_life_bar.gif", "qrc:/img/control_life_bar.gif"};
    return QMovie(list[generate<int>(0, list.size() - 1)]);
}

} // namespace Helper
