#include "charactersheet/rolisteamimageprovider.h"
#include <charactersheet_widgets/sheetwidget.h>

#include <charactersheet/charactersheet.h>
#include <charactersheet/charactersheetitem.h>
#include <charactersheet/csitem.h>

#include <QContextMenuEvent>
#include <QQmlContext>
#include <QQmlEngine>

SheetWidget::SheetWidget(CharacterSheet* chSheet, charactersheet::ImageModel* imgModel, QWidget* parent)
    : QQuickWidget(parent)
{
    auto engineQml= this->engine();
    auto imageProvider= new RolisteamImageProvider(imgModel);

    engineQml->addImageProvider(QLatin1String("rcs"), imageProvider);
    engineQml->addImportPath(QStringLiteral("qrc:/charactersheet/qml"));
    engineQml->addImportPath(QStringLiteral("qrc:/qml"));
    engineQml->addImportPath(QStringLiteral("qrc:/qml/rolistyle"));

    if(chSheet)
    {
        for(int i= 0; i < chSheet->getFieldCount(); ++i)
        {
            TreeSheetItem* field= chSheet->getFieldAt(i);
            if(nullptr != field)
            {
                engineQml->rootContext()->setContextProperty(field->id(), field);
            }
        }
    }
}

void SheetWidget::mousePressEvent(QMouseEvent* event)
{
    QQuickWidget::mousePressEvent(event);
    if(!event->isAccepted() && event->button() == Qt::RightButton && (event->modifiers() & Qt::ControlModifier))
    {
        emit customMenuRequested(event->pos());
    }
}
void SheetWidget::setSheet(CharacterSheet* sheet)
{
    if(sheet == m_characterSheet)
        return;

    m_characterSheet= sheet;
    emit sheetChanged();
}
CharacterSheet* SheetWidget::sheet() const
{
    return m_characterSheet;
}

QString SheetWidget::characterId() const
{
    return m_characterId;
}

void SheetWidget::setCharacterId(const QString& characterId)
{
    if(m_characterId == characterId)
        return;
    m_characterId= characterId;
    emit characterIdChanged();
}
