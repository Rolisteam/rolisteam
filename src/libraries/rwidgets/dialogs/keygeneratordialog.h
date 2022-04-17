#ifndef KEYGENERATORDIALOG_H
#define KEYGENERATORDIALOG_H

#include "rwidgets_global.h"
#include <QDialog>
namespace Ui
{
class KeyGeneratorDialog;
}

class RWIDGET_EXPORT KeyGeneratorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyGeneratorDialog(QWidget* parent= nullptr);
    ~KeyGeneratorDialog();

private slots:
    void generateKey();

private:
    Ui::KeyGeneratorDialog* ui;
};

#endif // KEYGENERATORDIALOG_H
