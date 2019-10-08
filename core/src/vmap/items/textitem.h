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
#include <QDialog>
#include <QFontMetrics>
#include <QGraphicsTextItem>
#include <QObject>
#include <QTextDocument>

class MRichTextEdit;

class TextLabel : public QGraphicsTextItem
{
public:
    TextLabel(QGraphicsItem* parent= nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
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
    MRichTextEdit* m_richText= nullptr;
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
    TextItem(const QPointF& start, quint16 penSize, const QColor& penColor, QGraphicsItem* parent= nullptr);
    /**
     * @brief paint the item into the scene.
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
    /**
     * @brief accessor to the bounding rect, helpful for focus and collision detection
     */
    virtual QRectF boundingRect() const override;
    /**
     * @brief amends the position of the end point, not really useful for this kind of graphical item.
     */
    virtual void setNewEnd(QPointF& nend) override;
    /**
     * @brief writeData
     * @param out
     */
    virtual void writeData(QDataStream& out) const override;
    /**
     * @brief readData
     * @param in
     */
    virtual void readData(QDataStream& in) override;

    /**
     * @brief getType
     * @return
     */
    virtual VisualItem::ItemType getType() const override;
    /**
     * @brief fillMessage
     * @param msg
     */
    virtual void fillMessage(NetworkMessageWriter* msg) override;
    /**
     * @brief readItem
     * @param msg
     */
    virtual void readItem(NetworkMessageReader* msg) override;

    /**
     * @brief setGeometryPoint
     * @param pointId
     * @param pos
     */
    virtual void setGeometryPoint(qreal pointId, QPointF& pos) override;
    /**
     * @brief initChildPointItem
     */
    virtual void initChildPointItem() override;
    void updateChildPosition() override;
    VisualItem* getItemCopy() override;
    void createActions() override;
    void addActionContextMenu(QMenu& menu) override;
    void setBorderVisible(bool);

    virtual void updateItemFlags() override;
    virtual void setRectSize(qreal x, qreal y, qreal w, qreal h) override;

    virtual void endOfGeometryChange() override;
    void setHoldSize(bool holdSize) override;
public slots:
    void updateTextPosition();
    void decreaseTextSize();
    void increaseTextSize();
    void editText();
    void sizeToTheContent();

protected:
    void wheelEvent(QGraphicsSceneWheelEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

private:
    void updateFont();
    void init();

private:
    QPointF m_start;
    QFont m_font;

    // QAction*
    QAction* m_increaseFontSize;
    QAction* m_decreaseFontSize;

    QTextDocument* m_doc;
    TextLabel* m_textItem;

    bool m_showRect;
    const QPointF m_offset;
    static RichTextEditDialog* m_dialog;
};

#endif // TEXTITEM_H
