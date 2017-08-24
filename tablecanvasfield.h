#ifndef TABLECANVASFIELD_H
#define TABLECANVASFIELD_H

#include <QObject>
#include "canvasfield.h"
#include "charactersheetitem.h"

#include "columndefinitiondialog.h"

class TableCanvasField;
class HandleItem : public QGraphicsObject
{
public:
    enum MOTION { X_AXIS, Y_AXIS};
    /**
     * @brief HandleItem
     * @param point
     * @param parent
     */
    HandleItem(QGraphicsObject* parent = nullptr);
    /**
     * @brief ~HandleItem
     */
    virtual ~HandleItem();
    /**
     * @brief itemChange
     * @param change
     * @param value
     * @return
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    /**
     * @brief boundingRect
     * @return
     */
    QRectF boundingRect() const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param widget
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    /**
     * @brief ChildPointItem::mouseMoveEvent
     * @param event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent * event);
    /**
     * @brief ChildPointItem::mouseReleaseEvent
     * @param event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

private:
   QPointF m_startPoint;
   TableCanvasField* m_parent;
   bool m_posHasChanged;
   MOTION m_currentMotion;
 };

class ButtonCanvas : public QGraphicsObject
{
    Q_OBJECT
public:
    ButtonCanvas();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString msg() const;
    void setMsg(const QString &msg);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

signals:
    void clicked();
private:
    QString m_msg;

};

class TableCanvasField : public CanvasField
{
        Q_OBJECT
public:
    TableCanvasField(Field* field);

    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    bool hasFocusOrChild();
public slots:
    void addColumn();
    void addLine();
    void defineColumns();


protected:
    virtual void setMenu(QMenu& menu);

private:
    int m_colunmCount;
    int m_lineCount;
    //QList<qreal> m_columnSize;
    QList<CharacterSheetItem::TypeField> m_fieldTypes;
    QList<HandleItem*> m_handles;

    ButtonCanvas* m_addColumn;
    ButtonCanvas* m_addLine;
    QAction* m_properties;
    QAction* m_defineColumns;

    ColumnDefinitionDialog* m_dialog;
    bool m_dataReset;

};

#endif // TABLECANVASFIELD_H
