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
#ifndef DICEMODEL_H
#define DICEMODEL_H

#include <QAbstractListModel>
#include <QColor>
#include <QQmlEngine>
#include <utility>

#include "dicecontroller.h"

class DiceModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged FINAL)
    Q_PROPERTY(int animationTime READ animationTime WRITE setAnimationTime NOTIFY animationTimeChanged FINAL)
public:
    enum Roles
    {
        Type= Qt::UserRole + 1,
        Color,
        Stable,
        Value,
        Ctrl,
    };
    Q_ENUM(Roles)

    explicit DiceModel(QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void addDice(DiceController::DiceType type, QColor color);
    void addDice(DiceController* ctrl);
    void removeDice(DiceController::DiceType type);
    int diceCount(DiceController::DiceType type) const;
    QList<DiceController*> selection() const;
    void setDiceData(const QByteArray& data);
    const std::vector<std::unique_ptr<DiceController>>& localModel() const;

    QSize size() const;
    void setSize(const QSize& newSize);

    static QByteArray diceControllerToData(const QList<DiceController*>& list, const QSize& size);
    static void fetchModel(const QByteArray& data, std::vector<std::unique_ptr<DiceController>>& model,
                           const QSize& size);
    static QHash<int, QList<int>> resultFromBytes(const QByteArray& data, QString& comment);

    int animationTime() const;
    void setAnimationTime(int newAnimationTime);

public slots:
    void checkStable();

signals:
    void selectionChanged();
    void diceRollChanged(QByteArray data);
    void sizeChanged();
    void diceCountChanged();

    void animationTimeChanged();

private:
    void setIndex(int idx);
    std::vector<std::unique_ptr<DiceController>>& currentModel() const;

private:
    int m_index{0};
    mutable std::array<std::vector<std::unique_ptr<DiceController>>, 2> m_models;
    QSize m_size;
    QTimer m_timer;
};

#endif // DICEMODEL_H
