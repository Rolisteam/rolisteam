#ifndef SHORTCUTEDITORDIALOG_H
#define SHORTCUTEDITORDIALOG_H

#include <QDialog>
class ShortCutModel;
namespace Ui {
class ShortCutEditorDialog;
}
class QAbstractItemModel;
class ShortCutEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortCutEditorDialog(QWidget *parent = 0);
    ~ShortCutEditorDialog();

    QAbstractItemModel* model() const;
    void setModel(QAbstractItemModel* model);

private:
    Ui::ShortCutEditorDialog *ui;
    QAbstractItemModel* m_model;
};

#endif // SHORTCUTEDITORDIALOG_H
