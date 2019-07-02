#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <QGraphicsView>

class ItemEditor : public QGraphicsView
{
    Q_OBJECT
public:
    ItemEditor(QWidget* parent= nullptr);

    bool handle() const;
    void setHandle(bool handle);

protected:
    void contextMenuEvent(QContextMenuEvent* event);

signals:
    void openContextMenu(QPoint);

private:
    bool m_handle;
};

#endif // ITEMEDITOR_H
