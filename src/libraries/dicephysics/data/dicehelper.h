#ifndef DICEHELPER_H
#define DICEHELPER_H

#include "dicecontroller.h"
#include <QList>
#include <QVector3D>
#include <QtMinMax>

namespace DiceHelper
{

struct RangeInfo
{
    int value;
    qreal beginX;
    qreal endX;
    qreal beginZ;
    qreal endZ;
};

template <typename T>
bool inBound(T a, T b, T c)
{
    auto v= qBound(a, b, c);
    return qFuzzyCompare(v, b);
}

template <DiceController::DiceType face>
QList<RangeInfo> values();

template <>
QList<RangeInfo> values<DiceController::DiceType::FOURSIDE>()
{
    return QList<RangeInfo>{{1, -29.3, -29.2, -112.4, -112.3},
                            {2, -27.9, -27.0, 112., 113.},
                            {3, 66.0, 67.0, 170., 180.},
                            {4, -0.5, 0.5, 0.01, 0.09}};
};

template <>
QList<RangeInfo> values<DiceController::DiceType::SIXSIDE>()
{
    // clang-format off
    return QList<RangeInfo>{{1, 89, 91, -0.5, 0.5},
                            {2, -0.5, 0.5, -180., -178},
                            {3, -0.5, 0.5, -92., -88.},
                            {4, -0.5, 0.5, 88, 90},
                            {5, -0.5, 0.5, -0.5, 0.5},
                            {6, -91, -89, -0.1, 0.1}};
    // clang-format on
};

template <>
QList<RangeInfo> values<DiceController::DiceType::OCTOSIDE>()
{
    // clang-format off
    return QList<RangeInfo>{{1, -36., -34, 43, 45},
                            {2, -36., -34., -136., -134},
                            {3, -36., -34., -46., -44.},
                            {4, -36, -34, 134., 136.},
                            {5, 34, 36, -46.0, -44.0},
                            {6, 34, 36, 134, 136},
                            {7, 34, 36, 44, 46},
                            {8, 34., 36., -135., -134.}};
    // clang-format on
};

template <>
QList<RangeInfo> values<DiceController::DiceType::TENSIDE>()
{
    // clang-format off
    return QList<RangeInfo>{{1, -33., -26, -146, -139},
                            {2, -12., -4., -48., -46.},
                            {3, 17., 24., -137., -133.},
                            {4, -50, -47, -20., -3.},
                            {5, 48., 50., 167., 176.0},
                            {6, -25., -17, 42., 46.},
                            {7, 4., 11., 132., 133.},
                            {8, 28., 32., 36., 39.},
                            {9, -41., -37., 150., 158.},
                            {10, 36., 42., -30., -20.}};
    // clang-format on
};

template <>
QList<RangeInfo> values<DiceController::DiceType::TWELVESIDE>()
{
    // clang-format off
    return QList<RangeInfo>{{1, -60., -57, 148, 152},
                            {2, -32., -31., 59., 61.},
                            {3, 58., 59., 149., 151.},
                            {4, -0.1, 0.1, 116., 124.},
                            {5, -32., -31., -121., -119.0},
                            {6, -0.1, 0.1, -180., -176.},
                            {7, 0., 0.1, 1., 2.},
                            {8, 30., 32., 58., 62.},
                            {9, 0., 0.1, -62., -60.},
                            {10, -60., -57., -32., -28.},
                            {11, 30., 32., -120., -118.},
                            {12, 58., 59., -31., -29.}};
    // clang-format on
};

template <>
QList<RangeInfo> values<DiceController::DiceType::TWENTYSIDE>()
{
    // clang-format off
    return QList<RangeInfo>{{1, -0.5, 0.5, 0., 1.},
                            {2, -1., 1.0, -138., -134.},
                            {3, 33., 37., 64., 68.},
                            {4, -71., -67., -160., -156},
                            {5, -22., -18., -70., -66},
                            {6, -23, -18, 110., 114.},
                            {7, 33., 37, -25., -19.},
                            {8, 33., 37., 154., 158.},
                            {9, -37., -33, 64., 68.},
                            {10, 67., 71., -160., -156.},
                            {11, -71., -67., 14., 23.},
                            {12, 33., 37., -115., -111.},
                            {13, -37., -34., -25., -21.0},
                            {14, -36, -34, 154., 158.},
                            {15, 18, 22, -70., -64},
                            {16, 18., 22., 109., 113.},
                            {17, 67., 71., 19., 23.},
                            {18, -37., -33., -115., -111.},
                            {19, -0.1, 0.1, 42., 46.},
                            {20, -1., 1., -181., -177.}};
    // clang-format on
};
template <>
QList<RangeInfo> values<DiceController::DiceType::ONEHUNDREDSIDE>()
{
    // clang-format off
    return QList<RangeInfo>{{10, -4., -3., 47, 48},
                            {20, 45., 48., -170., -165.},
                            {30, 38., 43., 20., 31.},
                            {40, 15., 23., 132., 137.},
                            {50, 28., 30., -42., -38.0},
                            {60, -34., -30, 140., 144.},
                            {70, -18., -14., -48., -44.},
                            {80, -39., -36., -153., -149.},
                            {90, -49., -46., 2., 11.},
                            {100, 2., 6., -134., -130.}};
    // clang-format on
};

template <DiceController::DiceType face>
int computeValue(const QVector3D& rotation)
{
    auto const& list= values<face>();
    int res= -1;

    for(auto l : list)
    {
        if(inBound(l.beginX, static_cast<qreal>(rotation.x()), l.endX)
           && inBound(l.beginZ, static_cast<qreal>(rotation.z()), l.endZ))
            return l.value;
    }

    // qDebug() << "error: no value found for:" << rotation;

    qreal min= 360 * 2;
    std::for_each(std::begin(list), std::end(list),
                  [&res, &min, rotation](const RangeInfo& a)
                  {
                      auto middleAX= (a.endX + a.beginX) / 2;
                      auto middleAZ= (a.endZ + a.beginZ) / 2;
                      auto distAX= rotation.x() - middleAX;
                      auto distAZ= rotation.z() - middleAZ;

                      auto distA= std::abs(distAZ) + std::abs(distAX);
                      min= std::min(min, distA);
                      if(qFuzzyCompare(min, distA))
                          res= a.value;
                  });

    // qDebug() << "result:" << res;
    return res;
}
} // namespace DiceHelper
#endif // DICEHELPER_H
