/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
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
#ifndef CHARACTERSHEETBUTTON_H
#define CHARACTERSHEETBUTTON_H

#include "charactersheetitem.h"
#include "csitem.h"
#include "field.h"
#include <QGraphicsItem>
#include <QLabel>
#include <QWidget>
/**
 * @brief The CharacterSheetButton class is dedicated to manage button.
 */
class CharacterSheetButton : public CSItem
{
    Q_OBJECT
public:
    CharacterSheetButton(QPointF topleft, QGraphicsItem* parent= nullptr);
    CharacterSheetButton(QGraphicsItem* parent= nullptr);

    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const override;
    virtual void setValueFrom(CharacterSheetItem::ColumnId id, QVariant var) override;

    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json, EditorController* ctrl) override;

    virtual void generateQML(QTextStream& out, CharacterSheetItem::QMLSection sec);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);

    virtual void setNewEnd(QPointF nend) override;
    QRectF boundingRect() const;

    CharacterSheetItem::TypeField getFieldType() const;

    virtual CharacterSheetItem* getChildAt(QString) const override;

    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const override;

    /**
     * @brief saveDataItem
     * @param json
     */
    virtual void saveDataItem(QJsonObject& json) override;
    /**
     * @brief load
     * @param json
     * @param scene
     */
    virtual void loadDataItem(const QJsonObject& json) override;

    void copyField(CharacterSheetItem* newField);
    /**
     * @brief fillNetworkMessage
     * @param msg
     */
    // virtual void fillNetworkMessage(NetworkMessageWriter* msg);
    /**
     * @brief readNetworkMessage
     * @param msg
     */
    // virtual void readNetworkMessage(NetworkMessageReader* msg);
signals:
    void updateNeeded(CSItem* c);

private:
    void init();
};

#endif // CHARACTERSHEETBUTTON_H
