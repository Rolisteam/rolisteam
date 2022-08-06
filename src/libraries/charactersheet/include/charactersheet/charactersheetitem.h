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
#ifndef CHARACTERSHEETITEM_H
#define CHARACTERSHEETITEM_H

#include <QJsonObject>
#include <QTextStream>
#include <QVariant>

#include <charactersheet/charactersheet_global.h>

class QGraphicsScene;
class EditorController;
/**
 * @brief The Item class
 */
class CHARACTERSHEET_EXPORT CharacterSheetItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QString formula READ getFormula WRITE setFormula NOTIFY formulaChanged)
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(CharacterSheetItemType itemType READ itemType CONSTANT)
public:
    enum CharacterSheetItemType
    {
        SectionItem,
        FieldItem,
        TableItem
    };

    enum ColumnId
    {
        ID,
        LABEL,
        VALUE,
        VALUES,
        TYPE,
        X,
        Y,
        WIDTH,
        HEIGHT,
        FitFont,
        FONT,
        TEXT_ALIGN,
        TEXTCOLOR,
        BGCOLOR,
        BORDER,
        PAGE,
        TOOLTIP
    };
    enum QMLSection
    {
        FieldSec,
        ConnectionSec
    };
    enum TypeField
    {
        TEXTINPUT,
        TEXTFIELD,
        TEXTAREA,
        SELECT,
        CHECKBOX,
        IMAGE,
        RLABEL,
        DICEBUTTON,
        FUNCBUTTON,
        WEBPAGE,
        NEXTPAGE,
        PREVIOUSPAGE,
        SLIDER,
        TABLE
    };

    CharacterSheetItem(CharacterSheetItem::CharacterSheetItemType type, QObject* parent= nullptr);

    virtual bool hasChildren();
    virtual int getChildrenCount() const;

    virtual CharacterSheetItem* getChildFromId(const QString& id) const= 0;
    virtual CharacterSheetItem* getChildAt(int) const;
    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId, int role) const;
    virtual void setValueFrom(CharacterSheetItem::ColumnId, QVariant data)= 0;
    virtual QString getPath();
    virtual bool mayHaveChildren() const;
    virtual void appendChild(CharacterSheetItem*);
    CharacterSheetItem* getParent() const;
    void setParent(CharacterSheetItem* parent);
    int rowInParent();
    virtual int indexOfChild(CharacterSheetItem* itm);
    virtual void save(QJsonObject& json, bool exp= false)= 0;
    virtual void load(const QJsonObject& json)= 0;
    virtual void saveDataItem(QJsonObject& json)= 0;
    virtual void loadDataItem(const QJsonObject& json)= 0;
    virtual void setNewEnd(QPointF nend)= 0;
    QString getId() const;
    void setId(const QString& id);
    virtual CharacterSheetItem::CharacterSheetItemType itemType() const;
    virtual bool removeChild(CharacterSheetItem*);
    virtual bool deleteChild(CharacterSheetItem*);

    Q_INVOKABLE QString getLabel() const;
    Q_INVOKABLE virtual QString value() const;
    Q_INVOKABLE bool isReadOnly() const;
    Q_INVOKABLE int page() const;
    Q_INVOKABLE QString getFormula() const;

    virtual void setFieldInDictionnary(QHash<QString, QString>& dict) const;
    bool hasFormula() const;

    CharacterSheetItem::TypeField getFieldType() const;
    void setCurrentType(const CharacterSheetItem::TypeField& currentType);

    CharacterSheetItem* getOrig() const;
    virtual void setOrig(CharacterSheetItem* orig);

    virtual void changeKeyChild(QString oldkey, QString newKey, CharacterSheetItem* child);
    QString getTooltip() const;
    void setTooltip(const QString& tooltip);

public slots:
    virtual void setValue(const QString& value, bool fromNetwork= false);
    void setReadOnly(bool readOnly);
    void setPage(int page);
    void setLabel(const QString& label);
    void setFormula(const QString& formula);
    void updateLabelFromOrigin();

signals:
    void valueChanged();
    void textColorChanged();
    void textAlignChanged();
    void bgColorChanged();
    void valuesChanged();
    void pageChanged();
    void readOnlyChanged();
    void formulaChanged();
    void idChanged();
    void labelChanged();
    void characterSheetItemChanged(CharacterSheetItem* item);

protected:
    CharacterSheetItem::CharacterSheetItemType m_itemType;
    CharacterSheetItem* m_parent;
    CharacterSheetItem* m_orig;
    int m_page;
    QString m_value;
    QString m_label;
    QString m_tooltip;
    bool m_readOnly;
    QString m_id;
    QString m_formula;
    TypeField m_currentType;
    bool m_hasDefaultValue;
};

#endif // CHARACTERSHEETITEM_H
