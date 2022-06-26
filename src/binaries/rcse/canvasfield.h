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
    Q_PROPERTY(FieldController* controller READ controller CONSTANT)
public:
    enum
    {
        Type= UserType + 1
    };
    CanvasField(FieldController* field);

    int type() const override { return Type; }

    FieldController* controller() const;

    QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget= nullptr) override;
    void setNewEnd(QPointF nend);

    static bool getShowImageField();
    static void setShowImageField(bool showImageField);
    virtual void setMenu(QMenu& menu);

protected:
    QPointer<FieldController> m_ctrl;
    QPixmap m_pix;
    int m_currentType;

    // static members
    static QHash<int, QString> m_pictureMap;
    static bool m_showImageField;
};

#endif // CANVASFIELD_H
