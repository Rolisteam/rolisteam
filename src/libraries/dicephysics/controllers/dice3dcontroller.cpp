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
#include "dice3dcontroller.h"

Dice3DController::Dice3DController(QObject* parent) : QObject{parent}, m_model(new DiceModel())
{
    m_colors[DiceController::DiceType::FOURSIDE]= QColor(255, 0, 0);
    m_colors[DiceController::DiceType::SIXSIDE]= QColor(255, 127, 0);
    m_colors[DiceController::DiceType::OCTOSIDE]= QColor(255, 255, 0);
    m_colors[DiceController::DiceType::TENSIDE]= QColor(0, 255, 0);
    m_colors[DiceController::DiceType::TWELVESIDE]= QColor(0, 0, 255);
    m_colors[DiceController::DiceType::TWENTYSIDE]= QColor(75, 0, 130);
    m_colors[DiceController::DiceType::ONEHUNDREDSIDE]= QColor(148, 0, 211);

    connect(this, &Dice3DController::fourColorChanged, this, &Dice3DController::colorChanged);
    connect(this, &Dice3DController::sixColorChanged, this, &Dice3DController::colorChanged);
    connect(this, &Dice3DController::eightColorChanged, this, &Dice3DController::colorChanged);
    connect(this, &Dice3DController::tenColorChanged, this, &Dice3DController::colorChanged);
    connect(this, &Dice3DController::twelveColorChanged, this, &Dice3DController::colorChanged);
    connect(this, &Dice3DController::twentyColorChanged, this, &Dice3DController::colorChanged);
    connect(this, &Dice3DController::oneHundredColorChanged, this, &Dice3DController::colorChanged);
    connect(this, &Dice3DController::countChanged, this, &Dice3DController::dicePartChanged);
    connect(m_model.get(), &DiceModel::selectionChanged, this, &Dice3DController::dicePartChanged);
    connect(m_model.get(), &DiceModel::sizeChanged, this, &Dice3DController::sizeChanged);
    connect(m_model.get(), &DiceModel::diceCountChanged, this, &Dice3DController::countChanged);
    connect(m_model.get(), &DiceModel::diceRollChanged, this, &Dice3DController::computeResult);
    connect(m_model.get(), &DiceModel::animationTimeChanged, this, &Dice3DController::animationTimeChanged);

    connect(this, &Dice3DController::hideTimeChanged, &m_timer,
            [this]() {
m_timer.setInterval(m_hideTime * 1000); });

    m_timer.setInterval(m_hideTime * 1000);

    auto restart= [this]() { m_timer.start(); };
    connect(this, &Dice3DController::colorChanged, this, restart);
    connect(this, &Dice3DController::dicePartChanged, this, restart);
    connect(this, &Dice3DController::sizeChanged, this, restart);
    connect(this, &Dice3DController::countChanged, this, restart);
    connect(this, &Dice3DController::expectRollChanged, this, restart);
    connect(this, &Dice3DController::animationTimeChanged, this, restart);
    connect(this, &Dice3DController::factorChanged, this, restart);
    connect(this, &Dice3DController::diceRolled, this, restart);
    connect(this, &Dice3DController::readyChanged, this, restart);
    connect(this, &Dice3DController::sharedOnlineChanged, this, restart);
    connect(this, &Dice3DController::hideTimeChanged, this, restart);
    connect(this, &Dice3DController::dicePartChanged, this, restart);
    connect(this, &Dice3DController::mutedChanged, this, restart);

    connect(this, &Dice3DController::displayedChanged, this,
            [this, restart]()
            {
                if(m_displayed)
                    restart();
                else
                    m_timer.stop();
            });

    connect(&m_timer, &QTimer::timeout, this, [this]() {
        setDisplayed(false);
    });
}

Dice3DController::~Dice3DController()
{
    qDebug() << "Destructor dice3D";
}

DiceModel* Dice3DController::model() const
{
    return m_model.get();
}

void Dice3DController::addDice(DiceController::DiceType type)
{
    m_model->addDice(type, diceColor(type));
    emit countChanged();
}

void Dice3DController::removeDice(DiceController::DiceType type)
{
    m_model->removeDice(type);
    emit countChanged();
}

QColor Dice3DController::diceColor(DiceController::DiceType type) const
{
    return m_colors[type];
}

int Dice3DController::diceCount(DiceController::DiceType type) const
{
    return m_model->diceCount(type);
}

void Dice3DController::computeResult(const QByteArray& doc)
{
    if(m_expectRoll && displayed())
    {
        QString comment;
        auto result= DiceModel::resultFromBytes(doc, comment);

        emit diceRolled(result, QString("%1 %2").arg(m_commandPart, comment));
        setExpectRoll(false);
    }
}

QString Dice3DController::diceTypeToCode(DiceController::DiceType type) const
{
    static QHash<DiceController::DiceType, QString> data{
        {DiceController::DiceType::FOURSIDE, "d4"},        {DiceController::DiceType::SIXSIDE, "d6"},
        {DiceController::DiceType::OCTOSIDE, "d8"},        {DiceController::DiceType::TENSIDE, "d10"},
        {DiceController::DiceType::TWELVESIDE, "d12"},     {DiceController::DiceType::TWENTYSIDE, "d20"},
        {DiceController::DiceType::ONEHUNDREDSIDE, "d100"}};

    return data.value(type);
}

QColor Dice3DController::fourColor() const
{
    return diceColor(DiceController::DiceType::FOURSIDE);
}

void Dice3DController::setFourColor(const QColor& newFourColor)
{
    if(fourColor() == newFourColor)
        return;
    m_colors[DiceController::DiceType::FOURSIDE]= newFourColor;
    emit fourColorChanged();
}

QColor Dice3DController::sixColor() const
{
    return diceColor(DiceController::DiceType::SIXSIDE);
}

void Dice3DController::setSixColor(const QColor& newSixColor)
{
    if(sixColor() == newSixColor)
        return;
    m_colors[DiceController::DiceType::SIXSIDE]= newSixColor;
    emit sixColorChanged();
}

QColor Dice3DController::eightColor() const
{
    return diceColor(DiceController::DiceType::OCTOSIDE);
}

void Dice3DController::setEightColor(const QColor& newEightColor)
{
    if(eightColor() == newEightColor)
        return;
    m_colors[DiceController::DiceType::OCTOSIDE]= newEightColor;
    emit eightColorChanged();
}

QColor Dice3DController::tenColor() const
{
    return diceColor(DiceController::DiceType::TENSIDE);
}

void Dice3DController::setTenColor(const QColor& newTenColor)
{
    if(tenColor() == newTenColor)
        return;
    m_colors[DiceController::DiceType::TENSIDE]= newTenColor;
    emit tenColorChanged();
}

QColor Dice3DController::twelveColor() const
{
    return diceColor(DiceController::DiceType::TWELVESIDE);
}

void Dice3DController::setTwelveColor(const QColor& newTwelveColor)
{
    if(twelveColor() == newTwelveColor)
        return;
    m_colors[DiceController::DiceType::TWELVESIDE]= newTwelveColor;
    emit twelveColorChanged();
}

QColor Dice3DController::twentyColor() const
{
    return diceColor(DiceController::DiceType::TWENTYSIDE);
}

void Dice3DController::setTwentyColor(const QColor& newTwentyColor)
{
    if(twentyColor() == newTwentyColor)
        return;
    m_colors[DiceController::DiceType::TWENTYSIDE]= newTwentyColor;
    emit twentyColorChanged();
}

QColor Dice3DController::oneHundredColor() const
{
    return diceColor(DiceController::DiceType::ONEHUNDREDSIDE);
}
void Dice3DController::setOneHundredColor(const QColor& oneHColor)
{
    if(oneHColor == oneHundredColor())
        return;
    m_colors[DiceController::DiceType::ONEHUNDREDSIDE]= oneHColor;
    emit oneHundredColorChanged();
}

bool Dice3DController::displayed() const
{
    return m_displayed;
}

void Dice3DController::setDisplayed(bool newDisplayed)
{
    if(m_displayed == newDisplayed)
        return;
    m_displayed= newDisplayed;
    emit displayedChanged();
}

QSize Dice3DController::size() const
{
    return m_model->size();
}

void Dice3DController::setSize(const QSize& newSize)
{
    m_model->setSize(newSize);
}

int Dice3DController::width() const
{
    return size().width();
}

int Dice3DController::height() const
{
    return size().height();
}

QPoint Dice3DController::position() const
{
    return m_position;
}

void Dice3DController::setPosition(QPoint newPosition)
{
    if(m_position == newPosition)
        return;
    m_position= newPosition;
    emit positionChanged();
}

int Dice3DController::x() const
{
    return m_position.x();
}

int Dice3DController::y() const
{
    return m_position.y();
}

bool Dice3DController::muted() const
{
    return m_muted;
}

void Dice3DController::setMuted(bool newMuted)
{
    if(m_muted == newMuted)
        return;
    m_muted= newMuted;
    emit mutedChanged();
}

QString Dice3DController::dicePart() const
{
    QStringList res;
    auto const& list= m_model->selection();
    QMap<DiceController::DiceType, int> map;

    for(auto const& p : list)
    {
        auto f= p->face();
        auto v= map.value(f, 0);
        map.insert(f, v + 1);
    }

    for(auto const& [k, v] : map.asKeyValueRange())
    {
        res << QString("%1d%2").arg(v).arg(static_cast<int>(k));
    }

    return res.join(';');
}

QString Dice3DController::commandPart() const
{
    return m_commandPart;
}

void Dice3DController::setCommandPart(const QString& newCommandPart)
{
    if(m_commandPart == newCommandPart)
        return;
    m_commandPart= newCommandPart;
    emit commandPartChanged();
}

bool Dice3DController::sharedOnline() const
{
    return m_sharedOnline;
}

void Dice3DController::setSharedOnline(bool newSharedOnline)
{
    if(m_sharedOnline == newSharedOnline)
        return;
    m_sharedOnline= newSharedOnline;
    emit sharedOnlineChanged();
}

qreal Dice3DController::factor() const
{
    return m_factor;
}

void Dice3DController::setFactor(qreal newFactor)
{
    if(qFuzzyCompare(m_factor, newFactor))
        return;
    m_factor= newFactor;
    emit factorChanged();
}

bool Dice3DController::ready() const
{
    return m_ready;
}

void Dice3DController::setReady(bool newReady)
{
    if(m_ready == newReady)
        return;
    m_ready= newReady;
    emit readyChanged();
}

bool Dice3DController::expectRoll() const
{
    return m_expectRoll;
}

void Dice3DController::setExpectRoll(bool newExpectRoll)
{
    if(m_expectRoll == newExpectRoll)
        return;
    m_expectRoll= newExpectRoll;
    emit expectRollChanged();
}

int Dice3DController::animationTime() const
{
    return m_model->animationTime();
}

void Dice3DController::setAnimationTime(int newAnimationTime)
{
    m_model->setAnimationTime(newAnimationTime);
}

int Dice3DController::hideTime() const
{
    return m_hideTime;
}

void Dice3DController::setHideTime(int newHideTime)
{
    if(m_hideTime == newHideTime || newHideTime < 10)
        return;
    m_hideTime= newHideTime;
    emit hideTimeChanged(m_hideTime);
}
