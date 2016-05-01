/***************************************************************************
* Copyright (C) 2014 by Renaud Guezennec                                   *
* http://www.rolisteam.org/                                                *
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

#include <QVariant>
#include <QJsonObject>
#include <QTextStream>

class QGraphicsScene;
/**
 * @brief The Item class
 */
class CharacterSheetItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ getId WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int page READ getPage WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(QString formula READ getFormula WRITE setFormula NOTIFY readOnlyChanged)
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged)
public:
    enum CharacterSheetItemType {SectionItem,FieldItem,ButtonItem};

    enum ColumnId {ID,LABEL,VALUE,VALUES,TYPE,X,Y,WIDTH,HEIGHT,CLIPPED,TEXT_ALIGN,TEXTCOLOR,BGCOLOR,BORDER};
    enum QMLSection {FieldSec,ConnectionSec};
    /**
     * @brief CharacterSheetItem
     */
    CharacterSheetItem();
    /**
     * @brief hasChildren
     * @return
     */
    virtual bool hasChildren();
    /**
     * @brief getChildrenCount
     * @return
     */
    virtual int getChildrenCount() const;
    /**
     * @brief getChildAt
     * @return
     */
    virtual CharacterSheetItem* getChildAt(QString) const=0;
    /**
     * @brief getChildAt
     * @return
     */
    virtual CharacterSheetItem* getChildAt(int) const;

    /**
     * @brief getValueFrom
     * @return
     */
    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId) const;
    /**
     * @brief setValueFrom
     * @param data
     */
    virtual void setValueFrom(CharacterSheetItem::ColumnId,QVariant data)=0;

    /**
     * @brief getPath
     * @return
     */
    virtual QString getPath();
    /**
     * @brief mayHaveChildren
     * @return
     */
    virtual bool mayHaveChildren();
    /**
     * @brief appendChild
     */
    virtual void appendChild(CharacterSheetItem*);
    /**
     * @brief getParent
     * @return
     */
    CharacterSheetItem *getParent() const;
    /**
     * @brief setParent
     * @param parent
     */
    void setParent(CharacterSheetItem *parent);
    /**
     * @brief rowInParent
     * @return
     */
    int rowInParent();
    /**
     * @brief indexOfChild
     * @param itm
     * @return
     */
    virtual int indexOfChild(CharacterSheetItem *itm);
    /**
     * @brief save
     * @param json
     * @param exp
     */
    virtual void save(QJsonObject& json,bool exp=false)=0;
    /**
     * @brief load
     * @param json
     * @param scene
     */
    virtual void load(QJsonObject& json,QList<QGraphicsScene*> scene)=0;

    /**
     * @brief save
     * @param json
     * @param exp
     */
    virtual void saveDataItem(QJsonObject& json)=0;
    /**
     * @brief load
     * @param json
     * @param scene
     */
    virtual void loadDataItem(QJsonObject& json)=0;
    /**
     * @brief generateQML
     * @param out
     * @param sec
     */
    virtual void generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec)=0;
    /**
     * @brief setNewEnd
     * @param nend
     */
    virtual void setNewEnd(QPointF nend)=0;
    /**
     * @brief getId
     * @return
     */
    QString getId() const;
    /**
     * @brief setId
     * @param id
     */
    void setId(const QString &id);
    /**
     * @brief getItemType
     * @return
     */
    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const = 0;
    /**
     * @brief getLabel
     * @return
     */
    Q_INVOKABLE QString getLabel() const;

    Q_INVOKABLE virtual QString value() const;
    Q_INVOKABLE bool isReadOnly() const;
    Q_INVOKABLE int getPage() const;

    Q_INVOKABLE QString getFormula() const;

public slots:
    /**
     * @brief setValue
     * @param value
     */
    virtual void setValue(const QString &value);
    /**
     * @brief setReadOnly
     * @param readOnly
     */
    void setReadOnly(bool readOnly);
    /**
     * @brief setPage
     * @param page
     */
    void setPage(int page);
    /**
     * @brief setLabel
     * @param label
     */
     void setLabel(const QString &label);

    void setFormula(const QString &formula);


signals:
    void valueChanged();
    void borderChanged();
    void textColorChanged();
    void textAlignChanged();
    void bgColorChanged();
    void valuesChanged();
    void pageChanged();
    void readOnlyChanged();
    void formulaChanged();
    void idChanged();
    void labelChanged();

protected:
    CharacterSheetItem* m_parent;
    int m_page;
    QString m_value;
    QString m_label;
    bool m_readOnly;
    QString m_id;
    QString m_formula;
};

#endif // CHARACTERSHEETITEM_H
