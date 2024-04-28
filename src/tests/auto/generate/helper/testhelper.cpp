#include "testhelper.h"

#include "rsrc_root_path.h"

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

namespace testhelper {

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
        res= {QIcon(":/img/Qt_Group_Community.jpg"), QIcon(":/img/qt-academy-potential.png"), QIcon(":/img/white.png")};
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
        res= {QUrl{"https://www.qt.io"}, QUrl{"https://blog.qt.io"}, QUrl{"https://google.com"},
               QUrl{"https://en.wikipedia.org"}, QUrl{"qrc:/img/qt-academy-potential.png"}};
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
        res= {QImage(":/img/qt_for_python_6_6_header.png"), QImage(":/img/Qt_Group_Our_Values.jpg"), QImage(":/img/white.png")};
        break;
    case QMetaType::QPixmap:
        res= {QPixmap(":/img/qt-academy-potential.png"), QPixmap(":/img/Qt_Group_Community.jpg"), QPixmap(":/img/white.png")};
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
        if(qMetaTypeId<std::vector<QPointF>>() == type.id())
        {
            res= {QVariant::fromValue(std::vector({QPointF(10., 10.), QPointF(100., 100.), QPointF(300., 100.)})),
                   QVariant::fromValue(std::vector({QPointF(500., 10.), QPointF(100., 400.), QPointF(10., 1000.)}))};
        }
        else if(qMetaTypeId<QList<QPointF>>() == type.id())
        {
            res= {QVariant::fromValue(QList({QPointF(10., 10.), QPointF(100., 100.), QPointF(300., 100.)})),
                   QVariant::fromValue(QList({QPointF(500., 10.), QPointF(100., 400.), QPointF(10., 1000.)}))};
        }
        else if(qMetaTypeId<Qt::CaseSensitivity>() == type.id())
        {
            res= {QVariant::fromValue(Qt::CaseInsensitive), QVariant::fromValue(Qt::CaseSensitive)};
        }
        else if(qMetaTypeId<QPainterPath>() == type.id())
        {
            res= {QVariant::fromValue(QPainterPath()), QVariant::fromValue(QPainterPath(QPointF(200., 100.))),
                   QVariant::fromValue(QPainterPath(QPointF(1200., 500.)))};
        }
    }
    if(res.empty() && testhelper::extraConvertorFunction)
    {
        res = extraConvertorFunction(type, currentValue);
    }

    if(res.removeOne(currentValue))
        res.append(currentValue);

    return res;
}



std::pair<bool, QStringList> testAllProperties(QObject* obj, bool setAgain)
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
            auto result = p.read(obj);
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
            for(const auto& v : vs)
            {
                if(!p.write(obj, v))
                    qDebug() << "write failed for " << p.name() << v;
                res= true;
            }

            spy.wait(10);

            auto count= spy.count();
            if(!QTest::qCompare(spy.count(), vs.size(), "spy.count()", "vs.size()", __FILE__, __LINE__))
            {
                qDebug() << p.name() << "values:" << vs << "current val:" << currentval;
            }

            if(setAgain)
            {
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
            p.write(obj, currentval);
        }
    }

    return std::make_pair(res, unmanaged);
}

QString imagePath(bool isSquare)
{
    QStringList array{	":/img/css-example.gif",
			":/img/qt-academy-potential.png",
			":/img/qt_for_python_6_6_header.png",
			":/img/Qt_Group_Community.jpg",
			":/img/Qt_Group_Our_Values.jpg",
			":/img/white.png"};

    if(isSquare)
        array= QStringList{":/img/arbre_square_500.jpg"};

    return array[generate<int>(0, array.size() - 1)];
}

QUrl imagePathUrl(bool isSquare)
{
    QList<QUrl> array{QUrl("qrc:/img/css-example.gif"),
	QUrl("qrc:/img/qt-academy-potential.png"),
	QUrl("qrc:/img/qt_for_python_6_6_header.png"),
	QUrl("qrc:/img/Qt_Group_Community.jpg"),
	QUrl("qrc:/img/Qt_Group_Our_Values.jpg"),
	QUrl("qrc:/img/white.png")};

    if(isSquare)
        array= QList<QUrl>{QUrl("qrc:/img/white.jpg")};

    return array[generate<int>(0, array.size() - 1)];
}

QByteArray loadFile(const QString& filepath)
{
    QByteArray data;
    if(filepath.isEmpty())
        return data;
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly))
    {
        data= file.readAll();
    }
    return data;
}

QByteArray imageData(bool isSquare)
{
    return loadFile(imagePath(isSquare));
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
                      static std::uniform_int_distribution<char> dist(0, std::numeric_limits<char>::max());
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
                          QStringLiteral("%1/resources/img/%2").arg(rsrc::root_path, url.path()));
                  });

    server->listen(QHostAddress::Any, port);

    return server;
}

QUrl randomUrl()
{
    static QStringList files{"css-example.gif",
		"qt-academy-potential.png",
		"qt_for_python_6_6_header.png",
		"Qt_Group_Community.jpg",
		"Qt_Group_Our_Values.jpg",
		"white.png"};
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
    static QStringList list{"qrc:/img/css-example.gif", "qrc:/img/css-example.gif"};
    return QMovie(list[generate<int>(0, list.size() - 1)]);
}


QList<std::pair<bool, QStringList> > inspectChildren(QObject *obj, QSet<QObject*>& alreadyVisited)
{
    if(alreadyVisited.contains(obj))
        return {};


    QList<std::pair<bool, QStringList>> res;
    alreadyVisited.insert(obj);

    res.append(testAllProperties(obj, true));
    auto const& children = obj->children();
    for(auto* child : children)
    {
        res << inspectChildren(child, alreadyVisited);
    }
    return res;
}

QList<std::pair<bool, QStringList> > inspectQObjectRecursively(QObject *obj)
{
    QSet<QObject*> alreadyVisited;


    auto res = inspectChildren(obj, alreadyVisited);

    for(auto const& obj : qAsConst(alreadyVisited))
    {
        auto meta = obj->metaObject();
        qDebug() << meta->className();
    }

    return res;
}

}
