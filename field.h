#ifndef FIELD_H
#define FIELD_H

#include <QWidget>
#include <QLabel>
#include <QGraphicsItem>
#include "item.h"

class Field : public QGraphicsObject,public Item
{
    Q_OBJECT
public:
    enum BorderLine {UP=1,LEFT=2,DOWN=4,RIGHT=8,ALL=15,NONE=16};
    enum TextAlign {ALignLEFT,ALignRIGHT,ALignCENTER};


    explicit Field(QGraphicsItem* parent = 0);
    explicit Field(QPointF topleft,QGraphicsItem* parent = 0);

    void writeQML();

    void drawField();
    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );


    virtual void setNewEnd(QPointF nend);

    QString key() const;
    void setKey(const QString &key);

    QPoint position() const;
    void setPosition(const QPoint &pos);

    QSize size() const;
    void setSize(const QSize &size);

    Field::BorderLine border() const;
    void setBorder(const Field::BorderLine &border);

    QColor bgColor() const;
    void setBgColor(const QColor &bgColor);

    QColor textColor() const;
    void setTextColor(const QColor &textColor);

    QFont font() const;
    void setFont(const QFont &font);

    QRectF boundingRect() const;


    virtual QVariant getValue(Item::ColumnId) const;
    virtual void setValue(Item::ColumnId id, QVariant var);

    virtual void save(QJsonObject& json);
    virtual void load(QJsonObject &json,QGraphicsScene* scene);

signals:
    void updateNeeded(Field* c);

protected:
    void init();
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent* ev);
private:
    QString m_key;
    BorderLine m_border;
    QColor m_bgColor;
    QColor m_textColor;
    QFont  m_font;
    TextAlign m_textAlign;

    //internal data
    QRectF m_rect;
    static int m_count;
};

#endif // FIELD_H
