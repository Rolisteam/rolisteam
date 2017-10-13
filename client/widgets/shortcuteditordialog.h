#ifndef SHORTCUTEDITORDIALOG_H
#define SHORTCUTEDITORDIALOG_H

#include <QDialog>
class ShortCutModel;
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
	ShortCutModel* m_model;
};

#endif // SHORTCUTEDITORDIALOG_H
