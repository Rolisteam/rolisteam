#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>

namespace Ui {
    class ServerDialog;
}
class PreferencesManager;
class ServerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerDialog(QWidget *parent = 0);
    ~ServerDialog();

    QString getPassword();
    bool hasPassword();
    int getPort();


    void setPassword(QString p);
    void enablePassword(bool p);
    void setPort(int p);

    void readPreferences();
    void writePrefences();

protected:
    void changeEvent(QEvent *e);


private:
    Ui::ServerDialog *ui;


    /**
      * pointer to the unique instance of preference manager.
      */
    PreferencesManager* m_options;
};

#endif // SERVERDIALOG_H
