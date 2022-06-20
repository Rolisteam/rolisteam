#ifndef CANVASFIELD_H
#define CANVASFIELD_H
#include <QGraphicsObject>
#include <QMenu>
#include <QPointer>
//
class FieldController;
/**
 * @brief The CanvasField class is the representation of the field into GraphicsScene.
 */
class CanvasField : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
public:
    enum
    {
        Type= UserType + 1
    };
    CanvasField(FieldController* field);

    int type() const override { return Type; }

    FieldController* getField() const;
    void setField(FieldController* field);

    QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
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
    QPointer<FieldController> m_field;
    QRectF m_rect;
    QPixmap m_pix;
    int m_currentType;
    bool m_locked= false;

    // static members
    static QHash<int, QString> m_pictureMap;
    static bool m_showImageField;
};

#endif // CANVASFIELD_H
