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
#include "charactersheet/csitem.h"
#include <QDebug>
#include <QTextDocument>

int CSItem::m_count= 0;
CSItem::CSItem(TreeSheetItem::TreeItemType type, QObject* parent, bool addCount)
    : TreeSheetItem(type, parent), m_rect(0, 0), m_page(0), m_hasDefaultValue(false)
{
    Q_UNUSED(parent);
    if(addCount)
    {
        ++m_count;
    }

    auto f= [this]
    {
        if(!m_fromNetwork)
            emit characterSheetItemChanged(this);
    };

    connect(this, &CSItem::xChanged, this, f);
    connect(this, &CSItem::yChanged, this, f);
    connect(this, &CSItem::widthChanged, this, f);
    connect(this, &CSItem::heightChanged, this, f);
    connect(this, &CSItem::borderChanged, this, f);
    connect(this, &CSItem::idChanged, this, f);
    connect(this, &CSItem::readOnlyChanged, this, f);
    connect(this, &CSItem::bgColorChanged, this, f);
    connect(this, &CSItem::formulaChanged, this, f);
    connect(this, &CSItem::pageChanged, this, f);
    connect(this, &CSItem::valueChanged, this, f);
}

QColor CSItem::bgColor() const
{
    return m_bgColor;
}

void CSItem::setBgColor(const QColor& bgColor)
{
    if(bgColor == m_bgColor)
        return;
    m_bgColor= bgColor;
    emit bgColorChanged();
}

QColor CSItem::textColor() const
{
    return m_textColor;
}

void CSItem::setTextColor(const QColor& textColor)
{
    if(textColor == m_textColor)
        return;
    m_textColor= textColor;
    emit textColorChanged();
}

qreal CSItem::x() const
{
    return m_pos.x();
}

qreal CSItem::y() const
{
    return m_pos.y();
}

qreal CSItem::width() const
{
    return m_rect.width();
}

qreal CSItem::height() const
{
    return m_rect.height();
}

void CSItem::setX(qreal x)
{
    if(qFuzzyCompare(m_pos.x(), x))
        return;
    m_pos.setX(x);
    emit xChanged();
}

void CSItem::setY(qreal y)
{
    if(qFuzzyCompare(m_pos.y(), y))
        return;
    m_pos.setY(y);
    emit yChanged();
}

void CSItem::setWidth(qreal width)
{
    if(qFuzzyCompare(width, m_rect.width()))
        return;
    m_rect.setWidth(width);
    emit widthChanged();
}

void CSItem::setHeight(qreal height)
{
    if(qFuzzyCompare(height, m_rect.height()))
        return;
    m_rect.setHeight(height);
    emit heightChanged();
}

CSItem::BorderLine CSItem::border() const
{
    return m_border;
}

void CSItem::setBorder(const CSItem::BorderLine& border)
{
    if(m_border == border)
        return;
    m_border= border;
    emit borderChanged();
}
void CSItem::resetCount()
{
    m_count= 0;
}
void CSItem::setCount(int i)
{
    m_count= i;
}
QString CSItem::value() const
{
    return m_value;
}

void CSItem::setValue(const QString& value, bool fromNetwork)
{
    if(m_readOnly && !fromNetwork)
        return;

    /// @warning ugly solution to prevent html rich text to break the change check.
    m_hasDefaultValue= false;
    QString newValue;
    QString currentValue;
    if(m_fieldType <= SELECT)
    {
        QTextDocument doc;
        doc.setHtml(value);
        newValue= doc.toPlainText();

        doc.setHtml(m_value);
        currentValue= doc.toPlainText();
    }
    else
    {
        newValue= value;
        currentValue= m_value;
    }

    if(currentValue == newValue)
        return;

    m_value= value;
    m_fromNetwork= fromNetwork;
    emit valueChanged(id());
    m_fromNetwork= false;
}

QString CSItem::label() const
{
    return m_label;
}

void CSItem::setLabel(const QString& label)
{
    if(m_label == label)
        return;

    m_label= label;
    emit labelChanged();
}

int CSItem::page() const
{
    return m_page;
}

void CSItem::setPage(int page)
{
    if(page == m_page)
        return;

    m_page= page;
    emit pageChanged();
}

QString CSItem::formula() const
{
    return m_formula;
}

void CSItem::setFormula(const QString& formula)
{
    if(formula == m_formula)
        return;
    m_formula= formula;
    emit formulaChanged();
}

CSItem* CSItem::orig() const
{
    return m_orig;
}

void CSItem::updateLabelFromOrigin()
{
    if(nullptr == m_orig)
        return;

    QString oldKey= m_label;

    setLabel(m_orig->label());

    if(nullptr != m_parent)
    {
        m_parent->changeKeyChild(oldKey, m_label, this);
    }
}

QString CSItem::getTooltip() const
{
    return m_tooltip;
}

void CSItem::setTooltip(const QString& tooltip)
{
    m_tooltip= tooltip;
}
void CSItem::setSecondPosition(QPointF nend)
{
    if(readOnly())
        return;

    setWidth(nend.x() /* - x()*/);
    setHeight(nend.y() /* - y()*/);

    emit widthChanged();
    emit heightChanged();
}
void CSItem::setOrig(TreeSheetItem* orig)
{
    auto o= dynamic_cast<CSItem*>(orig);
    if(o && m_orig != o)
    {
        m_orig= o;
        connect(m_orig, &CSItem::labelChanged, this, &CSItem::updateLabelFromOrigin);
    }
}
void CSItem::setFieldInDictionnary(QHash<QString, QString>& dict) const
{
    auto val= value();
    dict[m_id]= val;
    dict[label()]= val;
}
bool CSItem::hasFormula() const
{
    return !m_formula.isEmpty();
}
CSItem::TypeField CSItem::fieldType() const
{
    return m_fieldType;
}

void CSItem::setFieldType(const CSItem::TypeField& currentType)
{
    if(currentType == m_fieldType)
        return;
    m_fieldType= currentType;
    if(m_fieldType == CSItem::FUNCBUTTON && m_hasDefaultValue)
    {
        setValue("");
    }
    emit fieldTypeChanged();
}
