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
    Q_PROPERTY(QString characterId READ characterId WRITE setCharacterId NOTIFY characterIdChanged FINAL)
public:
    SheetWidget(CharacterSheet* chSheet, charactersheet::ImageModel* imgModel, QWidget* parent= nullptr);

    void setSheet(CharacterSheet* sheet);
    CharacterSheet* sheet() const;

    QString characterId() const;
    void setCharacterId(const QString& newPlayerId);

signals:
    void customMenuRequested(const QPoint pos);
    void sheetChanged();
    void characterIdChanged();

protected:
    virtual void mousePressEvent(QMouseEvent* event);

private:
    QPointer<CharacterSheet> m_characterSheet;
    QString m_characterId;
};

#endif // SHEETWIDGET_H
