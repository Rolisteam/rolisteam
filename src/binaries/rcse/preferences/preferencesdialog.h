#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    QString generationPath() const;
    void setGenerationPath(const QString &generationPath);

    bool hasCustomPath();
public slots:
    void selectDir();
private:
    Ui::PreferencesDialog *ui;
//    QString m_generationPath;
};

#endif // PREFERENCESDIALOG_H
