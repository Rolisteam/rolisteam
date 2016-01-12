#include "canvas.h"
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QUrl>




Canvas::Canvas(QObject *parent) : QGraphicsScene(parent),m_bg(NULL)
{

}

void Canvas::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    const QMimeData* mimeData =  event->mimeData();

    if(mimeData->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void Canvas::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    event->acceptProposedAction();
}


void Canvas::dropEvent ( QGraphicsSceneDragDropEvent * event )
{

    const QMimeData* mimeData =  event->mimeData();
    if(mimeData->hasUrls())
    {
        foreach(QUrl url, mimeData->urls())
        {
            if(url.isLocalFile())
            {
                m_bg = addPixmap(url.toLocalFile());
                setSceneRect(m_bg->boundingRect());
            }
        }
    }
}
void Canvas::setCurrentTool(Canvas::Tool tool)
{
    m_currentTool = tool;
}
void Canvas::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentTool==Canvas::MOVE)
    {
        if(mouseEvent->button() == Qt::LeftButton)
        {
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
    }
    else if(mouseEvent->button() == Qt::LeftButton)
    {
         if(m_currentTool==Canvas::ADD)
         {
            Field* field = new Field(mouseEvent->scenePos());
            addItem(field);
            m_model->appendField(field);
            m_currentItem = field;
         }

    }
    else if(mouseEvent->button()==Qt::RightButton)
    {
    }
}
void Canvas::mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(m_currentItem!=NULL)
    {
        m_currentItem->setNewEnd(mouseEvent->scenePos());
        update();
    }
    if(m_currentTool==Canvas::MOVE)
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}
void Canvas::mouseReleaseEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    Q_UNUSED(mouseEvent);

    m_currentItem = NULL;
    if(m_currentTool==Canvas::MOVE)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }
}
FieldModel *Canvas::model() const
{
    return m_model;
}

void Canvas::setModel(FieldModel *model)
{
    m_model = model;
}

QPixmap Canvas::pixmap()
{
    if(NULL!=m_bg)
    {
        return m_bg->pixmap();
    }
}

