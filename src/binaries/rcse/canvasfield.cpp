#include "canvasfield.h"

#include "charactersheet/controllers/fieldcontroller.h"
#include <QDebug>
#include <QGraphicsSceneContextMenuEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

QHash<int, QString> CanvasField::m_pictureMap({{FieldController::TEXTINPUT, ":/rcstyle/Actions-edit-rename-icon.png"},
                                               {FieldController::TEXTAREA, ":/rcstyle/textarea.png"},
                                               {FieldController::TEXTFIELD, ":/rcstyle/Actions-edit-rename-icon.png"},
                                               {FieldController::RLABEL, ""},
                                               {FieldController::SELECT, ""},
                                               {FieldController::SLIDER, ""},
                                               {FieldController::CHECKBOX, ":/rcstyle/checked_checkbox.png"},
                                               {FieldController::IMAGE, ":/rcstyle/photo.png"},
                                               {FieldController::WEBPAGE, ":/rcstyle/webPage.svg"},
                                               {FieldController::DICEBUTTON, ":/rcstyle/symbol.svg"}});

bool CanvasField::m_showImageField= true;

CanvasField::CanvasField(FieldController* field) : m_ctrl(field)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);

    connect(m_ctrl, &FieldController::readOnlyChanged, this,
            [this]()
            {
                setFlags(m_ctrl->readOnly() ? QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable :
                                              QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable
                                                  | QGraphicsItem::ItemIsFocusable);
            });

    connect(m_ctrl, &FieldController::xChanged, this,
            [this]()
            {
                setPos(m_ctrl->x(), m_ctrl->y());
                update();
            });
    connect(m_ctrl, &FieldController::yChanged, this,
            [this]()
            {
                setPos(m_ctrl->x(), m_ctrl->y());
                update();
            });
    connect(m_ctrl, &FieldController::widthChanged, this, [this]() { update(); });
    connect(m_ctrl, &FieldController::heightChanged, this, [this]() { update(); });
    connect(m_ctrl, &FieldController::destroyed, this, &CanvasField::deleteLater);
    connect(m_ctrl, &FieldController::destroyed, this,
            []()
            {
                qDebug() << "Controller destroyed";
                qDebug() << "Controller destroyed";
            });

    connect(this, &CanvasField::xChanged, m_ctrl, [this]() { m_ctrl->setX(pos().x()); });
    connect(this, &CanvasField::yChanged, m_ctrl, [this]() { m_ctrl->setY(pos().y()); });

    setVisible(m_ctrl->visible());
    connect(m_ctrl, &FieldController::visibleChanged, this, [this]() { setVisible(m_ctrl->visible()); });
}
void CanvasField::setNewEnd(QPointF nend)
{
    m_ctrl->setSecondPosition(nend);
}
FieldController* CanvasField::controller() const
{
    return m_ctrl;
}

QRectF CanvasField::boundingRect() const
{
    return QRectF{0, 0, m_ctrl->width(), m_ctrl->height()};
}
QPainterPath CanvasField::shape() const
{
    QPainterPath path;
    path.moveTo(0, 0);
    path.lineTo(m_ctrl->width(), 0);
    path.lineTo(m_ctrl->width(), m_ctrl->height());
    path.lineTo(0, m_ctrl->height());
    path.closeSubpath();
    return path;
}
void CanvasField::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget)

    if(nullptr == m_ctrl)
        return;
    painter->save();

    auto rect= boundingRect();

    painter->fillRect(rect, m_ctrl->bgColor());

    if(m_ctrl->readOnly())
        painter->setPen(Qt::gray);
    else
        painter->setPen(Qt::black);

    painter->drawRect(rect);

    if(option->state & QStyle::State_Selected)
    {
        painter->save();
        QPen pen= painter->pen();
        pen.setColor(Qt::red);
        pen.setWidth(5);
        painter->setPen(pen);
        painter->drawRect(rect);
        painter->restore();
    }

    int flags= 0;
    FieldController::TextAlign align= m_ctrl->textAlign();
    if(align < 3)
    {
        flags= Qt::AlignTop;
    }
    else if(align < 6)
    {
        flags= Qt::AlignVCenter;
    }
    else
    {
        flags= Qt::AlignBottom;
    }

    if(align % 3 == 0)
    {
        flags|= Qt::AlignRight;
    }
    else if(align % 3 == 1)
    {
        flags|= Qt::AlignHCenter;
    }
    else
    {
        flags|= Qt::AlignLeft;
    }

    if(m_pix.isNull() || m_currentType != m_ctrl->fieldType())
    {
        QPixmap map= QPixmap(m_pictureMap[m_ctrl->fieldType()]);
        if(!map.isNull())
        {
            m_pix= map.scaled(32, 32);
            m_currentType= m_ctrl->fieldType();
        }
    }
    if((!m_pix.isNull()) && m_showImageField)
    {
        painter->drawPixmap(rect.center() - m_pix.rect().center(), m_pix, m_pix.rect());
    }

    painter->drawText(rect, flags, m_ctrl->id());
    painter->restore();
}

bool CanvasField::getShowImageField()
{
    return m_showImageField;
}

void CanvasField::setShowImageField(bool showImageField)
{
    m_showImageField= showImageField;
}

void CanvasField::setMenu(QMenu& menu)
{
    Q_UNUSED(menu)
}
