/***************************************************************************
 *	Copyright (C) 2022 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DICE3DCONTROLLER_H
#define DICE3DCONTROLLER_H

#include "dicemodel.h"
#include <QObject>
#include <memory>
#include "dice3d_global.h"

class DICE3D_EXPORTS Dice3DController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(DiceModel* model READ model CONSTANT)
    Q_PROPERTY(QColor fourColor READ fourColor WRITE setFourColor NOTIFY fourColorChanged)
    Q_PROPERTY(QColor sixColor READ sixColor WRITE setSixColor NOTIFY sixColorChanged)
    Q_PROPERTY(QColor eightColor READ eightColor WRITE setEightColor NOTIFY eightColorChanged)
    Q_PROPERTY(QColor tenColor READ tenColor WRITE setTenColor NOTIFY tenColorChanged)
    Q_PROPERTY(QColor twelveColor READ twelveColor WRITE setTwelveColor NOTIFY twelveColorChanged)
    Q_PROPERTY(QColor twentyColor READ twentyColor WRITE setTwentyColor NOTIFY twentyColorChanged)
    Q_PROPERTY(QColor oneHundredColor READ oneHundredColor WRITE setOneHundredColor NOTIFY oneHundredColorChanged)
    Q_PROPERTY(bool displayed READ displayed WRITE setDisplayed NOTIFY displayedChanged FINAL)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged FINAL)
    Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged FINAL)
    Q_PROPERTY(int width READ width NOTIFY sizeChanged FINAL)
    Q_PROPERTY(int height READ height NOTIFY sizeChanged FINAL)
    Q_PROPERTY(int x READ x NOTIFY positionChanged FINAL)
    Q_PROPERTY(int y READ y NOTIFY positionChanged FINAL)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged FINAL)
    Q_PROPERTY(QString dicePart READ dicePart NOTIFY dicePartChanged FINAL)
    Q_PROPERTY(QString commandPart READ commandPart WRITE setCommandPart NOTIFY commandPartChanged FINAL)
    Q_PROPERTY(bool sharedOnline READ sharedOnline WRITE setSharedOnline NOTIFY sharedOnlineChanged FINAL)
    Q_PROPERTY(qreal factor READ factor WRITE setFactor NOTIFY factorChanged FINAL)
    Q_PROPERTY(bool ready READ ready WRITE setReady NOTIFY readyChanged FINAL)
    Q_PROPERTY(bool expectRoll READ expectRoll WRITE setExpectRoll NOTIFY expectRollChanged FINAL)
    Q_PROPERTY(int animationTime READ animationTime WRITE setAnimationTime NOTIFY animationTimeChanged FINAL)
public:
    explicit Dice3DController(QObject* parent= nullptr);

    DiceModel* model() const;

    Q_INVOKABLE void addDice(DiceController::DiceType type);
    Q_INVOKABLE void removeDice(DiceController::DiceType type);

    QColor fourColor() const;
    void setFourColor(const QColor& newFourColor);

    QColor sixColor() const;
    void setSixColor(const QColor& newSixColor);

    QColor eightColor() const;
    void setEightColor(const QColor& newEightColor);

    QColor tenColor() const;
    void setTenColor(const QColor& newTenColor);

    QColor twelveColor() const;
    void setTwelveColor(const QColor& newTwelveColor);

    QColor twentyColor() const;
    void setTwentyColor(const QColor& newTwentyColor);

    QColor oneHundredColor() const;
    void setOneHundredColor(const QColor& oneHColor);

    Q_INVOKABLE QColor diceColor(DiceController::DiceType type) const;
    Q_INVOKABLE int diceCount(DiceController::DiceType type) const;
    Q_INVOKABLE QString diceTypeToCode(DiceController::DiceType type) const;

    bool displayed() const;
    void setDisplayed(bool newDisplayed);

    QSize size() const;
    void setSize(const QSize& newSize);

    int width() const;
    int height() const;

    QPoint position() const;
    void setPosition(QPoint newPosition);

    int x() const;
    int y() const;

    bool muted() const;
    void setMuted(bool newMuted);

    QString dicePart() const;

    QString commandPart() const;
    void setCommandPart(const QString& newCommandPart);

    bool sharedOnline() const;
    void setSharedOnline(bool newSharedOnline);

    qreal factor() const;
    void setFactor(qreal newFactor);

    bool ready() const;
    void setReady(bool newReady);

    void computeResult(const QByteArray& doc);

    bool expectRoll() const;
    void setExpectRoll(bool newExpectRoll);

    int animationTime() const;
    void setAnimationTime(int newAnimationTime);

signals:
    void fourColorChanged();
    void sixColorChanged();
    void eightColorChanged();
    void tenColorChanged();
    void twelveColorChanged();
    void twentyColorChanged();
    void countChanged();
    void oneHundredColorChanged();
    void colorChanged();
    void displayedChanged();
    void sizeChanged();
    void positionChanged();
    void mutedChanged();
    void dicePartChanged();
    void commandPartChanged();
    void sharedOnlineChanged();
    void factorChanged();
    void readyChanged();
    void diceRolled(QHash<int, QList<int>> results, const QString& rest);

    void expectRollChanged();

    void animationTimeChanged();

private:
    std::unique_ptr<DiceModel> m_model;
    QHash<DiceController::DiceType, QColor> m_colors;
    bool m_displayed{false};
    QPoint m_position;
    bool m_muted{false};
    QString m_commandPart;
    bool m_sharedOnline;
    qreal m_factor;
    bool m_ready{false};
    bool m_expectRoll;
};

#endif // DICE3DCONTROLLER_H
