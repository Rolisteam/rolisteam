#ifndef KEYGENERATORDIALOG_H
#define KEYGENERATORDIALOG_H

#include <QDialog>

namespace Ui
{
    class KeyGeneratorDialog;
}

class KeyGeneratorDialog : public QDialog
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
