#ifndef SHEETWIDGET_H
#define SHEETWIDGET_H

#include <QMouseEvent>
#include <QPointF>
#include <QQuickWidget>

#include "charactersheet/charactersheet.h"
#include "charactersheet/imagemodel.h"
#include <charactersheet_widgets/charactersheet_widget_global.h>

class CHARACTERSHEET_WIDGET_EXPORT SheetWidget : public QQuickWidget
{
    Q_OBJECT
    Q_PROPERTY(CharacterSheet* sheet READ sheet WRITE setSheet NOTIFY sheetChanged)
public:
    SheetWidget(CharacterSheet* chSheet, charactersheet::ImageModel* imgModel, QWidget* parent= nullptr);

    void setSheet(CharacterSheet* sheet);
    CharacterSheet* sheet() const;

signals:
    void customMenuRequested(const QPoint pos);
    void sheetChanged();

protected:
    virtual void mousePressEvent(QMouseEvent* event);

private:
    CharacterSheet* m_characterSheet;
};

#endif // SHEETWIDGET_H
