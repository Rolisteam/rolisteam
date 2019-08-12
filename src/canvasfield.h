#ifndef CANVASFIELD_H
#define CANVASFIELD_H
#include <QGraphicsObject>
#include <QMenu>
//
class Field;
/**
 * @brief The CanvasField class is the representation of the field into GraphicsScene.
 */
class CanvasField : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
public:
    CanvasField(Field* field);

    Field* getField() const;
    void setField(Field* field);

    QRectF boundingRect() const;
    virtual QPainterPath shape() const;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr);
    void setNewEnd(QPointF nend);

    void setWidth(qreal w);
    void setHeight(qreal h);

    static bool getShowImageField();
    static void setShowImageField(bool showImageField);
    virtual void setMenu(QMenu& menu);
    bool locked() const;
public slots:
    void setLocked(bool b);

signals:
    void widthChanged();
    void heightChanged();
    void lockedChanged();

protected:
    Field* m_field;
    QRectF m_rect;
    static QHash<int, QString> m_pictureMap;
    static bool m_showImageField;
    QPixmap m_pix;
    int m_currentType;
    bool m_locked= false;
};

#endif // CANVASFIELD_H
