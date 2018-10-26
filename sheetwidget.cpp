#include "sheetwidget.h"

#include "charactersheet/charactersheet.h"
#include <QContextMenuEvent>

SheetWidget::SheetWidget(QWidget* parent)
    : QQuickWidget(parent)
{
}

void SheetWidget::mousePressEvent(QMouseEvent* event)
{
    QQuickWidget::mousePressEvent(event);
    if(!event->isAccepted() && event->button() == Qt::RightButton)
    {
        emit customMenuRequested(event->pos());
    }
}
void SheetWidget::setSheet(CharacterSheet* sheet)
{
    if(sheet == m_characterSheet)
        return;

    m_characterSheet = sheet;
    emit sheetChanged();
}
CharacterSheet* SheetWidget::sheet() const
{
    return m_characterSheet;
}
