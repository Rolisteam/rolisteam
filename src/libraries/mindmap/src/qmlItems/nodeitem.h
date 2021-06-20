#ifndef NODEITEM_H
#define NODEITEM_H

#include <QQuickItem>

class NodeItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QPointF position READ position WRITE setPosition NOTIFY positionChanged)
    /* */
public:
    NodeItem(QQuickItem* parent= nullptr);

    QString text();
    void setText(const QString& text);
    QColor color() const;
    void setColor(const QColor& color);
    QPointF position() const;
    void setPosition(const QPointF& origin);

signals:
    void textChanged();
    void colorChanged();
    void positionChanged();
    void contentWidthChanged();
    void contentHeightChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* node, UpdatePaintNodeData* updatePaintNodeData) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    void computeContentSize();

private:
    QString m_text;
    QColor m_color= Qt::gray;
    QPointF m_position;
    qreal m_contentWidth;
    qreal m_contentHeight;
};

#endif // NODEITEM_H
