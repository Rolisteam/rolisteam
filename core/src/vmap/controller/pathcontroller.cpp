#include "pathcontroller.h"

#include <QVariant>
namespace vmap
{
PathController::PathController(const std::map<QString, QVariant>& params, VectorialMapController* ctrl, QObject* parent)
    : VisualItemController(ctrl, parent)
{
    if(params.end() != params.find("color"))
        m_color= params.at(QStringLiteral("color")).value<QColor>();

    if(params.end() != params.find("tool"))
        m_penLine= (params.at(QStringLiteral("tool")).value<Core::SelectableTool>() == Core::SelectableTool::PEN);

    if(params.end() != params.find("penWidth"))
        m_penWidth= static_cast<quint16>(params.at(QStringLiteral("penWidth")).toInt());

    if(params.end() != params.find("position"))
        addPoint(params.at(QStringLiteral("position")).toPointF());
}

bool PathController::filled() const
{
    return m_filled;
}

bool PathController::closed() const
{
    return m_closed;
}

QColor PathController::color() const
{
    return m_color;
}

quint16 PathController::penWidth() const
{
    return m_penWidth;
}

int PathController::pointCount() const
{
    return static_cast<int>(m_points.size());
}

const std::vector<QPointF>& PathController::points() const
{
    return m_points;
}

QPointF PathController::pointAt(int corner) const
{
    if(corner != qBound(0, corner, pointCount()))
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
    emit pointCountChanged(static_cast<int>(m_points.size()));
    emit pointAdded(po, static_cast<int>(m_points.size()) - 1);
}

void PathController::aboutToBeRemoved()
{
    emit removeItem();
}

void PathController::endGeometryChange() {}

void PathController::setCorner(const QPointF& move, int corner)
{
    if(m_points.empty())
        return;
    if(corner != qBound(0, corner, static_cast<int>(m_points.size())))
        return;

    m_points[static_cast<std::size_t>(corner)]+= move;
    emit positionChanged(corner, m_points[static_cast<std::size_t>(corner)]);
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

void PathController::setColor(QColor color)
{
    if(m_color == color)
        return;

    m_color= color;
    emit colorChanged(m_color);
}

} // namespace vmap
