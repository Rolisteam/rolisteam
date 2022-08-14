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

#include "charactersheet/charactersheetitem.h"
#include "controllers/fieldcontroller.h"
#include "csitem.h"
#include <QGraphicsItem>
#include <QLabel>
#include <QWidget>
#include <charactersheet/charactersheet_global.h>
/**
 * @brief The CharacterSheetButton class is dedicated to manage button.
 */
/*class CHARACTERSHEET_EXPORT CharacterSheetButton : public CSItem
{
    Q_OBJECT
public:
    CharacterSheetButton(QPointF topleft, QGraphicsItem* parent= nullptr);
    CharacterSheetButton(QGraphicsItem* parent= nullptr);
    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const override;
    virtual void setValueFrom(CharacterSheetItem::ColumnId id, QVariant var) override;
    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json) override;
    virtual void generateQML(QTextStream& out, CharacterSheetItem::QMLSection sec);
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);
    virtual void setNewEnd(QPointF nend) override;
    QRectF boundingRect() const;
    CharacterSheetItem::TypeField getFieldType() const;
    virtual CharacterSheetItem* getChildFromId(const QString&) const override;
    virtual CharacterSheetItem::CharacterSheetItemType itemType() const override;
    virtual void saveDataItem(QJsonObject& json) override;
    virtual void loadDataItem(const QJsonObject& json) override;
    void copyField(CharacterSheetItem* newField);

signals:
    void updateNeeded(CSItem* c);

private:
    void init();
};
*/
#endif // CHARACTERSHEETBUTTON_H
