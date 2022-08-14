/***************************************************************************
 *   Copyright (C) 2015 by Renaud Guezennec                                *
 *   https://rolisteam.org/contact                   *
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
#ifndef CSITEM_H
#define CSITEM_H

#include "charactersheet/charactersheetitem.h"
#include <QColor>
#include <QPointF>
#include <QPointer>
#include <QRectF>
#include <QString>

#include <charactersheet/charactersheet_global.h>

class QGraphicsItem;
/**
 * @brief The CSItem class is managing some item values for RCSE.
 */
class CHARACTERSHEET_EXPORT CSItem : public TreeSheetItem
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QString formula READ formula WRITE setFormula NOTIFY formulaChanged)
    Q_PROPERTY(QString label READ label WRITE setLabel NOTIFY labelChanged)
    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(qreal height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(BorderLine border READ border WRITE setBorder NOTIFY borderChanged)
    Q_PROPERTY(QColor bgColor READ bgColor WRITE setBgColor NOTIFY bgColorChanged)

public:
    enum BorderLine
    {
        UP= 1,
        LEFT= 2,
        DOWN= 4,
        RIGHT= 8,
        ALL= 15,
        NONE= 16
    };
    Q_ENUM(BorderLine)
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
        SLIDER,
        HIDDEN,
        NEXTPAGE,
        PREVIOUSPAGE,
        TABLE
    };
    Q_ENUM(TypeField)
    CSItem(TreeSheetItem::TreeItemType type, QObject* parent= nullptr, bool addCount= true);

    QColor bgColor() const;
    QColor textColor() const;
    virtual qreal x() const;
    virtual qreal y() const;
    virtual qreal width() const;
    virtual qreal height() const;
    CSItem::BorderLine border() const;
    bool hasFormula() const;

    QString label() const;
    QString value() const;
    bool isReadOnly() const;
    int page() const;
    QString formula() const;
    virtual void setSecondPosition(QPointF nend);
    virtual QVariant valueFrom(TreeSheetItem::ColumnId, int role) const;
    virtual void setValueFrom(TreeSheetItem::ColumnId, const QVariant& data);

    CSItem::TypeField fieldType() const;
    void setFieldType(const CSItem::TypeField& currentType);

    QString getTooltip() const;
    void setTooltip(const QString& tooltip);

    CSItem* orig() const;
    void setOrig(TreeSheetItem* orig) override;

    static void resetCount();
    static void setCount(int i);

    virtual void setFieldInDictionnary(QHash<QString, QString>& dict) const override;
public slots:
    void setBgColor(const QColor& bgColor);
    void setTextColor(const QColor& textColor);
    virtual void setX(qreal x);
    virtual void setY(qreal y);
    virtual void setWidth(qreal width);
    virtual void setHeight(qreal height);
    void setBorder(const CSItem::BorderLine& border);
    virtual void setValue(const QString& value, bool fromNetwork= false);
    void setReadOnly(bool readOnly);
    void setPage(int page);
    void setLabel(const QString& label);
    void setFormula(const QString& formula);
    void updateLabelFromOrigin();

signals:
    void xChanged();
    void yChanged();
    void widthChanged();
    void heightChanged();
    void borderChanged();
    void valueChanged();
    void textColorChanged();
    void textAlignChanged();
    void bgColorChanged();
    void valuesChanged();
    void pageChanged();
    void readOnlyChanged();
    void formulaChanged();
    void labelChanged();
    void idChanged();

protected:
    QSize m_rect;
    QPointF m_pos;
    QColor m_bgColor;
    QColor m_textColor;
    BorderLine m_border;
    QPointF m_posPrivate;
    int m_page;
    QString m_value;
    QString m_label;
    QString m_tooltip;
    QString m_formula;
    bool m_readOnly;
    bool m_hasDefaultValue;
    TypeField m_fieldType;
    QPointer<CSItem> m_orig;
    bool m_fromNetwork= false;

    static int m_count;
};

#endif // CSITEM_H
