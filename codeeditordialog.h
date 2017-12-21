#ifndef CODEEDITORDIALOG_H
#define CODEEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class CodeEditorDialog;
}

class CodeEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CodeEditorDialog(QWidget *parent = 0);
    ~CodeEditorDialog();

private:
    Ui::CodeEditorDialog *ui;
};

#endif // CODEEDITORDIALOG_H
