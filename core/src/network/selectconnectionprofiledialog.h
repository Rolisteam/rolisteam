#ifndef SELECTCONNECTIONPROFILEDIALOG_H
#define SELECTCONNECTIONPROFILEDIALOG_H

#include "connectionprofile.h"
#include "data/player.h"
#include <QDialog>
#include <QSettings>

namespace Ui
{
class SelectConnectionProfileDialog;
}

/**
 * @brief The SelectConnectionProfileDialog class is the dialog box shown at starting or when the connection is lost.
 */
class SelectConnectionProfileDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief SelectConnectionProfileDialog
     * @param parent
     */
    explicit SelectConnectionProfileDialog(QString version, QWidget* parent= nullptr);
    /**
     * @brief ~SelectConnectionProfileDialog
     */
    virtual ~SelectConnectionProfileDialog();

    /**
     * @brief getSelectedProfile
     * @return
     */
    ConnectionProfile* getSelectedProfile();

    /**
     * Load informations from the previous rolisteam's execution
     */
    void readSettings();

    /**
     * Save parameters for next executions.
     */
    void writeSettings();

    /**
     * @brief setArgumentProfile
     * @param host
     * @param port
     * @param password
     */
    void setArgumentProfile(QString host, int port, QByteArray password);
    /**
     * @brief updateGUI
     */
    void updateGUI();
signals:
    /**
     * @brief tryConnection
     */
    void tryConnection();
public slots:
    /**
     * @brief removeProfile
     */
    void removeProfile();
    /**
     * @brief updateProfile
     */
    void updateProfile();
    /**
     * @brief setCurrentProfile
     */
    void setCurrentProfile(QModelIndex);
    void connectTo();
    void connectToIndex(QModelIndex index);
    void openImage();
    void errorOccurs(QString);
    void checkConnection();
    void disconnection();
    /**
     * @brief endOfConnectionProcess
     */
    void endOfConnectionProcess();

private:
    Ui::SelectConnectionProfileDialog* ui;
    ProfileModel* m_model;
    ConnectionProfile* m_currentProfile;
    QString m_version;
    QString m_avatarUri;
    bool m_passChanged= false;
};

#endif // SELECTCONNECTIONPROFILEDIALOG_H
