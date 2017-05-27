#ifndef SHEETPROPERTIES_H
#define SHEETPROPERTIES_H

#include <QDialog>

namespace Ui {
class SheetProperties;
}

class SheetProperties : public QDialog
{
    Q_OBJECT

public:
    explicit SheetProperties(QWidget *parent = 0);
    ~SheetProperties();

private:
    Ui::SheetProperties *ui;
};

#endif // SHEETPROPERTIES_H
