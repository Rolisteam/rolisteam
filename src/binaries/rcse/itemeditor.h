#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <QGraphicsView>
#include <QList>
#include <QPointer>
#include <memory>

class EditorController;
class FieldController;
class ItemEditor : public QGraphicsView
{
    Q_OBJECT
public:
    ItemEditor(QWidget* parent= nullptr);
    void setController(EditorController* editCtrl);
    bool handle() const;
    void setHandle(bool handle);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

signals:
    void openContextMenu(QPoint);

private:
    bool m_handle;
    QPointer<EditorController> m_ctrl;
    std::unique_ptr<QAction> m_lockItem;
    std::unique_ptr<QAction> m_fitInView;
    std::unique_ptr<QAction> m_alignOnY;
    std::unique_ptr<QAction> m_alignOnX;
    std::unique_ptr<QAction> m_sameWidth;
    std::unique_ptr<QAction> m_sameHeight;
    std::unique_ptr<QAction> m_dupplicate;
    std::unique_ptr<QAction> m_verticalEquaDistance;
    std::unique_ptr<QAction> m_horizontalEquaDistance;

    QList<FieldController*> m_selection;
    QPointer<FieldController> m_underCursorItem;
};

#endif // ITEMEDITOR_H
