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

#ifdef RCSE
#include "canvasfield.h"
#else
class CHARACTERSHEET_EXPORT CanvasField : public QGraphicsObject
{
    CanvasField();
};
#endif

/**
 * @brief The Field class managed text field in qml and datamodel.
 */
class CHARACTERSHEET_EXPORT FieldController : public CSItem
{
    Q_OBJECT
    Q_PROPERTY(bool fitFont READ fitFont WRITE setFitFont NOTIFY fitFontChanged)
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

    explicit FieldController(bool addCount= true, QGraphicsItem* parent= nullptr);
    explicit FieldController(QPointF topleft, bool addCount= true, QGraphicsItem* parent= nullptr);
    virtual ~FieldController();

    QSize size() const;
    void setSize(const QSize& size);

    QFont font() const;
    void setFont(const QFont& font);

    CharacterSheetItem* getChildFromId(const QString& id) const override;

    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const override;
    virtual void setValueFrom(CharacterSheetItem::ColumnId id, QVariant var) override;

    virtual void save(QJsonObject& json, bool exp= false) override;
    virtual void load(const QJsonObject& json, EditorController* ctrl) override;
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

    virtual QPointF mapFromScene(QPointF) override;

    QStringList getAvailableValue() const;
    void setAvailableValue(const QStringList& availableValue);

    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const override;

    void copyField(CharacterSheetItem*, bool copyData, bool sameId= true);

    bool fitFont() const;

    void setTextAlign(const TextAlign& textAlign);

    FieldController::TextAlign getTextAlignValue();

    virtual void setNewEnd(QPointF nend) override;

    CanvasField* getCanvasField() const;
    virtual void setCanvasField(CanvasField* canvasField);

    void initGraphicsItem() override;

    virtual qreal getWidth() const override;
    virtual void setWidth(qreal width) override;

    virtual qreal getHeight() const override;
    virtual void setHeight(qreal height) override;

    virtual void setX(qreal x) override;
    virtual qreal getX() const override;

    virtual void setY(qreal x) override;
    virtual qreal getY() const override;

    QString getGeneratedCode() const;
    void setGeneratedCode(const QString& generatedCode);

    bool getAliasEnabled() const;
    void setAliasEnabled(bool aliasEnabled);

    QPair<QString, QString> getTextAlign();
    bool isLocked() const;
public slots:
    void setLocked(bool b);
    void storeQMLCode();
    void setFitFont(bool clippedText);
signals:
    void updateNeeded(CSItem* c);
    void fitFontChanged(bool b);

protected:
    void init();
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent* ev);

protected:
    QFont m_font;
    TextAlign m_textAlign= CenterMiddle;
    QStringList m_availableValue;
    bool m_fitFont= false;
    CanvasField* m_canvasField;
    QString m_generatedCode;
    bool m_aliasEnabled= true;
    bool m_locked= false;
};

#endif // FIELD_H
