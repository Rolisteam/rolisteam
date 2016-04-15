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
public:
    enum CharacterSheetItemType {SectionItem,FieldItem,ButtonItem};

    enum ColumnId {ID,LABEL,VALUE,X,Y,WIDTH,HEIGHT,BORDER,TEXT_ALIGN,BGCOLOR,TEXTCOLOR,VALUES,TYPE,CLIPPED};
    enum QMLSection {FieldSec,ConnectionSec};
    CharacterSheetItem();
    virtual bool hasChildren();
    virtual int getChildrenCount() const;

    virtual CharacterSheetItem* getChildAt(QString) const=0;
    virtual CharacterSheetItem* getChildAt(int) const;


    virtual QVariant getValueFrom(CharacterSheetItem::ColumnId) const;
    virtual void setValueFrom(CharacterSheetItem::ColumnId,QVariant data)=0;


    virtual QString getPath();

    virtual bool mayHaveChildren();
    virtual void appendChild(CharacterSheetItem*);
    CharacterSheetItem *getParent() const;
    void setParent(CharacterSheetItem *parent);

    int rowInParent();

    virtual int indexOfChild(CharacterSheetItem *itm);
    virtual void save(QJsonObject& json,bool exp=false)=0;
    virtual void load(QJsonObject& json,QList<QGraphicsScene*> scene)=0;

    virtual void generateQML(QTextStream& out,CharacterSheetItem::QMLSection sec)=0;
    virtual void setNewEnd(QPointF nend)=0;

    QString getId() const;
    void setId(const QString &id);

    virtual CharacterSheetItem::CharacterSheetItemType getItemType() const = 0;

    QString getLabel() const;
    void setLabel(const QString &label);

    Q_INVOKABLE virtual QString value() const;
    Q_INVOKABLE bool isReadOnly() const;
    Q_INVOKABLE int getPage() const;

public slots:
    virtual void setValue(const QString &value);
    void setReadOnly(bool readOnly);
    void setPage(int page);



signals:
    void valueChanged();
    void borderChanged();
    void textColorChanged();
    void textAlignChanged();
    void bgColorChanged();
    void valuesChanged();
    void pageChanged();
    void readOnlyChanged();
    void idChanged();



protected:
    CharacterSheetItem* m_parent;
    int m_page;
    QString m_value;
    QString m_label;
    bool m_readOnly;
    QString m_id;
};

#endif // CHARACTERSHEETITEM_H
