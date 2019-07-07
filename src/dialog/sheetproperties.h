#ifndef SHEETPROPERTIES_H
#define SHEETPROPERTIES_H

#include <QDialog>
#include <QStringListModel>
namespace Ui
{
class SheetProperties;
}
class QmlGeneratorController;
class SheetProperties : public QDialog
{
    Q_OBJECT

public:
    explicit SheetProperties(QmlGeneratorController* m_ctrl, QWidget* parent= nullptr);
    virtual ~SheetProperties() override;

    void init();
public slots:
    void accept() override;

private:
    Ui::SheetProperties* ui;
    QStringList m_fontUri;
    QStringListModel m_model;
    QmlGeneratorController* m_ctrl;
};

#endif // SHEETPROPERTIES_H
