#ifndef SHEETWIDGET_H
#define SHEETWIDGET_H

#include <QQuickWidget>
#include <QPointF>
#include <QMouseEvent>

class CharacterSheet;
class SheetWidget : public QQuickWidget
{
    Q_OBJECT
    Q_PROPERTY(CharacterSheet* sheet READ sheet WRITE setSheet NOTIFY sheetChanged)
public:
    SheetWidget(QWidget* parent = nullptr);

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
