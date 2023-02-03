#include "helper.h"
#include "media/mediatype.h"
#include "network/networkmessagewriter.h"

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
#include <QPixmap>
#include <QPolygon>
#include <QPolygonF>
#include <QRect>
#include <QRectF>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTest>
#include <QUrl>
#include <QUuid>
#include <QVariant>
#include <QtHttpServer>
#include "test_root_path.h"
#include "utils/iohelper.h"
#include <limits>

namespace Helper
{
QVariantList values(QMetaType type, QVariant currentValue)
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
              std::numeric_limits<char16_t>::max(), std::numeric_limits<char16_t>::min()};
        break;
    case QMetaType::Char32:
        res= {static_cast<char32_t>('A'), static_cast<char32_t>('1'), static_cast<char32_t>('z'),
              std::numeric_limits<char32_t>::max(), std::numeric_limits<char32_t>::min()};
        break;
    case QMetaType::Bool:
        res= {true, false};
        break;
    case QMetaType::Int:
        res= {std::numeric_limits<int>::max(), std::numeric_limits<int>::min()};
        break;
    case QMetaType::UInt:
        res= {std::numeric_limits<uint>::max(), std::numeric_limits<uint>::min()};
        break;
    case QMetaType::Double:
        res= {std::numeric_limits<double>::max(), std::numeric_limits<double>::min()};
        break;
    case QMetaType::QChar:
        res= {QChar('A'), QChar('1'), QChar('z')};
        break;
    case QMetaType::QString:
        res= {QString("aaaa"), QString("zzzz"), QString()};
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
        res= {QRectF(10., 10., 10., 10.), QRectF(100., 100., 100., 100.), QRectF(1., 1., 1., 1.), QRectF()};
        break;
    case QMetaType::QLine:
        res= {QLine(10, 10, 20, 20), QLine(100, 100, 200, 200), QLine()};
        break;
    case QMetaType::QStringList:
        res= {QStringList{"aaa", "bbbb", "cccc"}, QStringList{"zzz", "yyyyy", "xxxxx"}};
        break;
    case QMetaType::QIcon:
        res= {QIcon("qrc:/img/arbre_square_500.jpg"), QIcon("qrc:/img/lion.jpg"), QIcon("qrc:/img/white.png")};
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

        res= {QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont),
              QFontDatabase::systemFont(QFontDatabase::FixedFont),
              QFontDatabase::systemFont(QFontDatabase::GeneralFont),
              QFontDatabase::systemFont(QFontDatabase::TitleFont)};

        break;
    case QMetaType::QBrush:
        res= {QBrush{Qt::red}, QBrush{Qt::black}, QBrush{Qt::green}};
        break;
    case QMetaType::QImage:
        res= {QImage("qrc:/img/arbre_square_500.jpg"), QImage("qrc:/img/lion.jpg"), QImage("qrc:/img/white.png")};
        break;
    case QMetaType::QPixmap:
        res= {QPixmap("qrc:/img/arbre_square_500.jpg"), QPixmap("qrc:/img/lion.jpg"), QPixmap("qrc:/img/white.png")};
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

std::pair<bool, QStringList> testAllProperties(QObject* obj)
{
    if(!obj)
        return {};
    auto meta= obj->metaObject();
    bool res= false;
    QStringList unmanaged;
    for(int i= meta->propertyOffset(); i < meta->propertyCount(); ++i)
    {
        auto p= meta->property(i);
        if(p.isConstant())
        {
            p.read(obj);
            continue;
        }

        if(p.hasNotifySignal() && p.isWritable())
        {
            QSignalSpy spy(obj, p.notifySignal());

            auto vs= values(p.metaType(), p.read(obj));

            if(vs.isEmpty())
            {
                unmanaged.append(p.name());
                continue;
            }

            for(const auto& v : vs)
            {
                if(!p.write(obj, v))
                    qDebug() << "write failed for " << p.name() << v;
                res= true;
            }

            spy.wait(10);

            // QCOMPARE(spy.count(), vs.size())

            auto count= spy.count();
            if(!QTest::qCompare(spy.count(), vs.size(), "spy.count()", "vs.size()", __FILE__, __LINE__))
                qDebug() << p.name();

            for(const auto& v : vs)
            {
                if(!p.write(obj, v))
                {
                    qDebug() << "write failed for " << p.name() << v;

                    spy.wait(10);
                    QTest::qVerify(count == spy.count(), "Signal emitted when received same value",
                                   "Signal emitted when received same value", __FILE__, __LINE__);
                }
            }
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

QByteArray imageData(bool isSquare)
{
    return utils::IOHelper::loadFile(imagePath(isSquare));
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
    auto server = new QHttpServer();

    server->route("/image/<arg>", [] (const QUrl &url) {
        return QHttpServerResponse::fromFile(QStringLiteral("%1/resources/img/%2").arg(tests::root_path, url.path()));
    });

    server->listen( QHostAddress::Any, port);

    return server;
}

} // namespace Helper
