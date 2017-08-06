#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <QGraphicsView>


class ItemEditor : public QGraphicsView
{
    Q_OBJECT
public:
    ItemEditor(QWidget* parent = nullptr);

    bool handle() const;
    void setHandle(bool handle);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

signals:
    void openContextMenu(QPoint);

private:
    bool m_handle;
};

#endif // ITEMEDITOR_H
