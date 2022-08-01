#include "controller/item_controllers/pathcontroller.h"

#include <QVariant>
namespace vmap
{

QPainterPath vectorToPath(const std::vector<QPointF>& points, bool closeUp= false)
{
    QPainterPath path;
    bool first= true;
    QPointF start;
    for(QPointF p : points)
    {
        if(first)
        {
            first= false;
            start= p;
            path.moveTo(p);
        }
        else
            path.lineTo(p);
    }

    if(closeUp)
        path.lineTo(start);
    return path;
}
PathController::PathController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(VisualItemController::PATH, params, ctrl, parent)
{
    if(params.end() != params.find(Core::vmapkeys::KEY_TOOL))
    {
        m_tool= params.at(Core::vmapkeys::KEY_TOOL).value<Core::SelectableTool>();
        m_penLine= (m_tool == Core::SelectableTool::PEN);
    }
    else if(params.end() != params.find(Core::vmapkeys::KEY_PENLINE))
    {
        m_penLine= params.at(Core::vmapkeys::KEY_PENLINE).toBool();
        m_tool= m_penLine ? Core::SelectableTool::PEN : Core::SelectableTool::PATH;
    }

    if(params.end() != params.find(Core::vmapkeys::KEY_FILLED))
        m_filled= params.at(Core::vmapkeys::KEY_FILLED).toBool();

    if(params.end() != params.find(Core::vmapkeys::KEY_CLOSED))
        m_closed= params.at(Core::vmapkeys::KEY_CLOSED).toBool();

    if(params.end() != params.find(Core::vmapkeys::KEY_POINTS))
        m_points= params.at(Core::vmapkeys::KEY_POINTS).value<std::vector<QPointF>>();

    if(params.end() != params.find(Core::vmapkeys::KEY_PENWIDTH))
        m_penWidth= static_cast<quint16>(params.at(Core::vmapkeys::KEY_PENWIDTH).toInt());

    if(!m_penLine)
        addPoint(QPointF(0, 0));

    connect(this, &PathController::pointsChanged, this, [this] { setModified(); });
    connect(this, &PathController::pointCountChanged, this, [this] { setModified(); });
    connect(this, &PathController::penWidthChanged, this, [this] { setModified(); });
    connect(this, &PathController::closedChanged, this, [this] { setModified(); });
    connect(this, &PathController::filledChanged, this, [this] { setModified(); });
}

bool PathController::filled() const
{
    return m_filled;
}

bool PathController::closed() const
{
    return m_closed;
}

quint16 PathController::penWidth() const
{
    return m_penWidth;
}

quint64 PathController::pointCount() const
{
    return static_cast<quint64>(m_points.size());
}

const std::vector<QPointF>& PathController::points() const
{
    return m_points;
}

QPointF PathController::pointAt(quint64 corner) const
{
    if(corner != qBound(static_cast<quint64>(0), corner, pointCount()))
        return {};

    return m_points[static_cast<std::size_t>(corner)];
}

bool PathController::penLine() const
{
    return m_penLine;
}
void PathController::addPoint(const QPointF& po)
{
    m_points.push_back(po);
    m_pointPositonEditing= true;
    emit pointCountChanged(static_cast<int>(m_points.size()));
    emit pointAdded(po, static_cast<int>(m_points.size()) - 1);
}

QPainterPath PathController::path() const
{
    return vectorToPath(points(), closed());
}

void PathController::aboutToBeRemoved()
{
    emit removeItem();
}

void PathController::endGeometryChange()
{
    VisualItemController::endGeometryChange();

    if(m_pointPositonEditing && m_penLine)
    {
        setInitialized(true);
        m_pointPositonEditing= false;
        emit pointCountChanged(static_cast<int>(m_points.size()));
    }
    else if(m_pointPositonEditing)
    {
        m_pointPositonEditing= false;
        emit pointPositionEditFinished(m_modifiedPointIdx, pointAt(static_cast<quint64>(m_modifiedPointIdx)));
        m_modifiedPointIdx= -1;
    }
}

void PathController::setCorner(const QPointF& move, int corner,
                               Core::TransformType tt)
{
    if(m_points.empty())
        return;
    if(corner != qBound(0, corner, static_cast<int>(m_points.size())))
        return;

    m_points[static_cast<std::size_t>(corner)]+= move;
    emit positionChanged(corner, m_points[static_cast<std::size_t>(corner)]);
    m_pointPositonEditing= true;
    m_modifiedPointIdx= corner;
}

void PathController::setPoint(const QPointF& p, int corner)
{
    if(m_points.empty())
        return;
    if(corner != qBound(0, corner, static_cast<int>(m_points.size())))
        return;

    m_points[static_cast<std::size_t>(corner)]= p;
    emit positionChanged(corner, m_points[static_cast<std::size_t>(corner)]);
}

QRectF PathController::rect() const
{
    return path().boundingRect();
}

void PathController::setFilled(bool filled)
{
    if(m_filled == filled)
        return;

    m_filled= filled;
    emit filledChanged(m_filled);
}

void PathController::setClosed(bool closed)
{
    if(m_closed == closed)
        return;

    m_closed= closed;
    emit closedChanged(m_closed);
}

void PathController::setPoints(const std::vector<QPointF>& points)
{
    m_points.clear();
    m_points= points;
    emit pointsChanged();
}

} // namespace vmap
