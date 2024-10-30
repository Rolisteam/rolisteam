#include <algorithm>

#include "helper_global.h"
#include "network/networkmessage.h"
// #include "utils/iohelper.h"
#include <QByteArray>
#include <QColor>
#include <QPointF>
#include <QRectF>
#include <QStringList>
#include <QUrl>
#include <QMovie>
#include <map>
#include <random>
#include <type_traits>
#include <utility>

class QObject;
namespace Helper
{
template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
    /* Credits: Mark Nelson http://marknelson.us */
    if((first == last) || (first == k) || (last == k))
        return false;
    Iterator i1= first;
    Iterator i2= last;
    ++i1;
    if(last == i1)
        return false;
    i1= last;
    --i1;
    i1= k;
    --i2;
    while(first != i1)
    {
        if(*--i1 < *i2)
        {
            Iterator j= k;
            while(!(*i1 < *j))
                ++j;
            std::iter_swap(i1, j);
            ++i1;
            ++j;
            i2= k;
            std::rotate(i1, j, last);
            while(last != j)
            {
                ++j;
                ++i2;
            }
            std::rotate(k, i2, last);
            return true;
        }
    }
    std::rotate(first, k, last);
    return false;
}

class HELPER_EXPORT TestMessageSender : public MessageSenderInterface
{
public:
    TestMessageSender();
    virtual void sendMessage(const NetworkMessage* msg) override;

    QByteArray messageData() const;

private:
    QByteArray m_msgData;
};

HELPER_EXPORT std::pair<bool, QStringList> testAllProperties(QObject* obj, QStringList ignoredProperties, bool setAgain= true);

template <typename T>
T generate(const T& min, const T& max) //=std::numeric_limits<T>::min(),
                                       //=std::numeric_limits<T>::max()
{
    static std::random_device dev;
    static std::mt19937 rng(dev());
    /*
      using dist_t = std::conditional<std::is_floating_point_v<T>,
                                      std::uniform_real_distribution<T>,
                                      std::uniform_int_distribution<T>>;*/
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
HELPER_EXPORT const std::map<QString, QVariant> buildRectController(bool filled, const QRectF& rect,
                                                                    const QPointF& pos= QPointF(0, 0));

HELPER_EXPORT const std::map<QString, QVariant>
buildTextController(bool border, const QString& text, const QRectF& rect, const QPointF& pos= QPointF(0, 0));

HELPER_EXPORT const std::map<QString, QVariant> buildEllipseController(bool filled, qreal rx, qreal ry,
                                                                       const QPointF& pos= QPointF(0, 0));

HELPER_EXPORT const std::map<QString, QVariant> buildImageController(const QString& path, const QRectF& rect,
                                                                     const QPointF& pos= QPointF(0, 0));

HELPER_EXPORT const std::map<QString, QVariant> buildPenController(bool filled, const std::vector<QPointF>& points,
                                                                   const QPointF& pos);

HELPER_EXPORT const std::map<QString, QVariant> buildPathController(bool filled, const std::vector<QPointF>& points,
                                                                    const QPointF& pos= QPointF(0, 0));
HELPER_EXPORT const std::map<QString, QVariant> buildLineController(const QPointF& p1, const QPointF& p2,
                                                                    const QPointF& pos= QPointF(0, 0));
HELPER_EXPORT const std::map<QString, QVariant> buildTokenController(bool isNpc, const QPointF& pos);

QString randomString(int length= 10)
{
    QString res;
    static QString list{"abcdefghijklmnropqrstuvwxyzABCDEFGHIJKLMOPQRSTUVWXYZ1234567890"};

    for(int i= 0; i < length; ++i)
    {
        res.append(list[generate<int>(0, list.size() - 1)]);
    }

    return res;
}

HELPER_EXPORT QString imagePath(bool isSquare= false);
HELPER_EXPORT QUrl imagePathUrl(bool isSquare = false);
HELPER_EXPORT QByteArray imageData(bool isSquare= false);
HELPER_EXPORT QByteArray randomData(int length= 1000);
HELPER_EXPORT QColor randomColor();
HELPER_EXPORT QPointF randomPoint();
HELPER_EXPORT QRectF randomRect();
HELPER_EXPORT QObject* initWebServer(int port= 9090);
HELPER_EXPORT QUrl randomUrl();
HELPER_EXPORT QMovie randomMovie();

template <typename T>
T randomFromList(const std::vector<T>& list)
{
    return list[generate<int>(0, list.size() - 1)];
}
} // namespace Helper
