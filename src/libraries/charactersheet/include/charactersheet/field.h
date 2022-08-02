/***************************************************************************
 * Copyright (C) 2014 by Renaud Guezennec                                   *
 * https://rolisteam.org/                                                *
 *                                                                          *
 *  This file is part of rcse                                               *
 *                                                                          *
 * rcse is free software; you can redistribute it and/or modify             *
 * it under the terms of the GNU General Public License as published by     *
 * the Free Software Foundation; either version 2 of the License, or        *
 * (at your option) any later version.                                      *
 *                                                                          *
 * rcse is distributed in the hope that it will be useful,                  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
 * GNU General Public License for more details.                             *
 *                                                                          *
 * You should have received a copy of the GNU General Public License        *
 * along with this program; if not, write to the                            *
 * Free Software Foundation, Inc.,                                          *
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.                 *
 ***************************************************************************/
#ifndef FIELD_H
#define FIELD_H

#include <QFont>
#include <QGraphicsObject>
#include <QMouseEvent>

#include "charactersheet/charactersheetitem.h"
#include "csitem.h"
#include <charactersheet/charactersheet_global.h>

class CanvasField;
/**
 * @brief The Field class managed text field in qml and datamodel.
 */
class CHARACTERSHEET_EXPORT FieldController : public CSItem
{
    Q_OBJECT
    Q_PROPERTY(bool fitFont READ fitFont WRITE setFitFont NOTIFY fitFontChanged)
    Q_PROPERTY(FieldController::TextAlign textAlign READ textAlign WRITE setTextAlign NOTIFY textAlignChanged)
    Q_PROPERTY(QString generatedCode READ generatedCode WRITE setGeneratedCode NOTIFY generatedCodeChanged)
    Q_PROPERTY(bool aliasEnabled READ aliasEnabled WRITE setAliasEnabled NOTIFY aliasEnabledChanged)
    Q_PROPERTY(QStringList availableValues READ availableValues WRITE setAvailableValues NOTIFY availableValuesChanged)
public:
    enum TextAlign
    {
        TopRight,
        TopMiddle,
        TopLeft,
        CenterRight,
        CenterMiddle,
        CenterLeft,
        BottomRight,
        BottomMiddle,
        BottomLeft
    };

    explicit FieldController(CharacterSheetItem::CharacterSheetItemType itemType
                             = CharacterSheetItem::CharacterSheetItemType::FieldItem,
                             bool addCount= true, QGraphicsItem* parent= nullptr);
    explicit FieldController(CharacterSheetItem::CharacterSheetItemType itemType
                             = CharacterSheetItem::CharacterSheetItemType::FieldItem,
                             QPointF topleft= {}, bool addCount= true, QGraphicsItem* parent= nullptr);
    virtual ~FieldController();

    QFont font() const;
    bool fitFont() const;
    FieldController::TextAlign textAlign();
    QString generatedCode() const;
    bool aliasEnabled() const;
    QStringList availableValues() const;

    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const override;
    virtual void setValueFrom(CharacterSheetItem::ColumnId id, QVariant var) override;

    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json) override;
    virtual void saveDataItem(QJsonObject& json) override;
    virtual void loadDataItem(const QJsonObject& json) override;

    void copyField(CharacterSheetItem*, bool copyData, bool sameId= true);
    virtual void setNewEnd(QPointF nend) override;
    QPair<QString, QString> getTextAlign();
public slots:
    // void storeQMLCode();
    void setGeneratedCode(const QString& generatedCode);
    void setAliasEnabled(bool aliasEnabled);
    void setAvailableValues(const QStringList& availableValue);
    void setFitFont(bool clippedText);
    void setFont(const QFont& font);
    void setTextAlign(const FieldController::TextAlign& textAlign);

signals:
    void updateNeeded(CSItem* c);
    void fitFontChanged(bool b);
    void generatedCodeChanged();
    void aliasEnabledChanged();
    void availableValuesChanged();

protected:
    void init();

protected:
    QFont m_font;
    TextAlign m_textAlign= CenterMiddle;
    QStringList m_availableValue;
    bool m_fitFont= false;
    QString m_generatedCode;
    bool m_aliasEnabled= true;
};

#endif // FIELD_H
