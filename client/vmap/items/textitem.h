/***************************************************************************
    *      Copyright (C) 2010 by Renaud Guezennec                             *
    *                                                                         *
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
#ifndef TEXTITEM_H
#define TEXTITEM_H
#include "visualitem.h"
#include <QObject>
#include <QFontMetrics>
#include <QGraphicsTextItem>
#include <QTextDocument>
#include "widgets/MRichTextEditor/mrichtextedit.h"
#include <QDialog>
class TextLabel : public QGraphicsTextItem
{
public:
    TextLabel(QGraphicsItem* parent = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

};
/**
 * @brief The RichTextEditDialog class is a simple class based on QDialog to display rich text editor
 */
class RichTextEditDialog : public QDialog
{
public:
    RichTextEditDialog();

    void setText(QString str);
    QString getText();

private:
    MRichTextEdit* m_richText;

};

/**
    * @brief displays and manages text on map, part of QGraphicsScene/view.
    */
class TextItem : public VisualItem
{
    Q_OBJECT
public:
    TextItem();
    /**
    * @brief Constructor with parameters
    * @param start, starting point, it represents the bottom right rectangle corner where the text willbe displayed
    */
    TextItem(QPointF& start,quint16 penSize,QColor& penColor,QGraphicsItem * parent = 0);
    /**
    * @brief paint the item into the scene.
    */
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    /**
    * @brief accessor to the bounding rect, helpful for focus and collision detection
    */
    virtual QRectF boundingRect() const ;
    /**
    * @brief amends the position of the end point, not really useful for this kind of graphical item.
    */
    virtual void setNewEnd(QPointF& nend);
    /**
     * @brief writeData
     * @param out
     */
    virtual void writeData(QDataStream& out) const;
    /**
     * @brief readData
     * @param in
     */
    virtual void readData(QDataStream& in);
    
    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType() const;
    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg);
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg);

    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId, QPointF &pos);
    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem();
    void updateChildPosition();
	VisualItem* getItemCopy();
    void createActions();
    void addActionContextMenu(QMenu* menu);
    void setBorderVisible(bool);

    virtual void setEditableItem(bool b);
    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h);

    virtual void endOfGeometryChange();
public slots:
    void updateTextPosition();
    void decreaseTextSize();
    void increaseTextSize();
    void editText();
    void sizeToTheContent();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    void updateFont();
    void init();

private:
    QPointF m_start;
    QFont m_font;

    //QAction*
    QAction* m_increaseFontSize;
    QAction* m_decreaseFontSize;

    QTextDocument* m_doc;
    TextLabel* m_textItem;

    bool m_showRect;
    const QPointF m_offset;
    static RichTextEditDialog* m_dialog;

};

#endif // TEXTITEM_H
