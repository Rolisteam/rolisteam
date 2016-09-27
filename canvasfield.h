#ifndef CANVASFIELD_H
#define CANVASFIELD_H
#include <QGraphicsObject>

//
class Field;

class CanvasField : public QGraphicsObject
{
    Q_OBJECT
public:
    CanvasField(Field* field);

    Field* getField() const;
    void setField(Field* field);

    QRectF boundingRect() const;
    virtual QPainterPath shape() const;

    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
    void setNewEnd(QPointF nend);
signals:
    void positionChanged();

private:
    Field* m_field;
    QRectF m_rect;
};

#endif // CANVASFIELD_H
