#ifndef DICECONTROLLER_H
#define DICECONTROLLER_H

#include <QColor>
#include <QElapsedTimer>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QVector3D>
#include <memory>
#include <tuple>

class DiceController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Type managed by cpp")
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged FINAL)
    Q_PROPERTY(DiceController::DiceType face READ face CONSTANT)
    Q_PROPERTY(int value READ value NOTIFY valueChanged FINAL)
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged FINAL)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged FINAL)
    Q_PROPERTY(bool stable READ stable WRITE setStable NOTIFY stableChanged FINAL)
    Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged FINAL)
public:
    enum class DiceType
    {
        FOURSIDE= 4,
        SIXSIDE= 6,
        OCTOSIDE= 8,
        TENSIDE= 10,
        TWELVESIDE= 12,
        TWENTYSIDE= 20,
        ONEHUNDREDSIDE= 100
    };
    Q_ENUM(DiceType)
    explicit DiceController(DiceController::DiceType face, QObject* parent= nullptr);

    QColor color() const;
    void setColor(const QColor& newColor);

    DiceController::DiceType face() const;

    int value() const;

    QVector3D position() const;
    void setPosition(const QVector3D& newPosition);

    QVector3D rotation() const;
    void setRotation(const QVector3D& newRotation);

    bool stable() const;
    void setStable(bool newStable);

    bool selected() const;
    void setSelected(bool newSelected);

    std::tuple<quint64, float, float> unstableData() const;

public slots:
    void elapsed();
    void addRotationOffset(float r);

signals:
    void colorChanged();
    void valueChanged();
    void positionChanged();
    void rotationChanged();
    void stableChanged();
    void selectedChanged();

private:
    void setValue(int v);

private:
    QColor m_color;
    DiceController::DiceType m_face;
    int m_value{0};
    QVector3D m_position;
    QVector3D m_rotation;
    bool m_stable{true};
    bool m_selected{false};
    QElapsedTimer m_count;
    std::unique_ptr<QTimer> m_timer;

    quint64 m_unstableTime{0};
    float m_unstableDistance{0.f};
    float m_unstableRotation{0.f};
    QVector3D m_unstablePosition;
};

#endif // DICECONTROLLER_H
