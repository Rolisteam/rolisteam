#ifndef TABLECANVASFIELD_H
#define TABLECANVASFIELD_H

#include <QObject>
#include "canvasfield.h"
#include "charactersheetitem.h"
#include "columndefinitiondialog.h"

class TableCanvasField;
class LineModel;
class TableField;
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
    void load(QJsonObject &json);
    void save(QJsonObject &json);

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
    QRectF rect() const;
    void setRect(const QRectF &rect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

signals:
    void clicked();
private:
    QString m_msg;
    QRectF m_rect;
};

class TableCanvasField : public CanvasField
{
        Q_OBJECT
public:
    TableCanvasField(Field* field);
    virtual ~TableCanvasField();

    void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );

    bool hasFocusOrChild();

    int colunmCount() const;
    void setColunmCount(int colunmCount);

    int lineCount() const;
    void setLineCount(int lineCount);

    int getColumnWidth(int c);
    int getLineHeight();

    void fillLineModel(LineModel* model, TableField* parent);

    Field* generateSubField(int i);

    void generateSubFields(QTextStream &out);

    void load(QJsonObject &json, QList<QGraphicsScene *> scene);
    void save(QJsonObject &json);

    void setPositionAddLine(int pos);
    int getPosition() const;

public slots:
    void addColumn();
    void removeColumn();
    void addLine();
    void defineColumns();

protected:
    virtual void setMenu(QMenu& menu);

private:
    int m_colunmCount;
    int m_lineCount;

    QList<HandleItem *> m_handles;

    ButtonCanvas* m_addColumn;
    ButtonCanvas* m_addLine;
    ButtonCanvas* m_addLineInGame;
    QAction* m_defineColumns;

    ColumnDefinitionDialog* m_dialog;
    bool m_dataReset;
    int m_position;
    bool m_columnDefined = false;


};

#endif // TABLECANVASFIELD_H
