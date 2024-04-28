#ifndef TESTHELPER_H
#define TESTHELPER_H

#include <map>
#include <random>
#include <QString>
#include <QUrl>
#include <QColor>
#include <QPointF>
#include <QRect>
#include <QMovie>
#include <functional>

namespace testhelper
{
extern std::function<QVariantList(QMetaType type, QVariant currentValue)> extraConvertorFunction;

template <typename T>
T generate(const T& min, const T& max)
{
    static std::random_device dev;
    static std::mt19937 rng(dev());

    T res;
    if constexpr(std::is_integral<T>::value)
    {
        std::uniform_int_distribution<T> dist(min, max);
        res= dist(rng);
    }
    else if(std::is_floating_point<T>::value)
    {
        std::uniform_real_distribution<T> dist(min, max);
        res= dist(rng);
    }
    else
    {
        res= T{}; // useless
    }
    return res;
}

QList<std::pair<bool, QStringList>> inspectQObjectRecursively(QObject* obj);
std::pair<bool, QStringList> testAllProperties(QObject* obj, bool setAgain= true);
QString imagePath(bool isSquare= false);
QUrl imagePathUrl(bool isSquare = false);
QByteArray imageData(bool isSquare= false);
QByteArray randomData(int length= 1000);
QColor randomColor();
QPointF randomPoint();
QRectF randomRect();
QObject* initWebServer(int port= 9090);
QUrl randomUrl();
QMovie randomMovie();

template <typename T>
T randomFromList(const std::vector<T>& list)
{
    return list[generate<int>(0, list.size() - 1)];
}
} // namespace PropertyTest
#endif // TESTHELPER_H
