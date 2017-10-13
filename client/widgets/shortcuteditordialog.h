#ifndef SHORTCUTEDITORDIALOG_H
#define SHORTCUTEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class ShortCutEditorDialog;
}

class ShortCutEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortCutEditorDialog(QWidget *parent = 0);
    ~ShortCutEditorDialog();

private:
    Ui::ShortCutEditorDialog *ui;
};

#endif // SHORTCUTEDITORDIALOG_H
