#ifndef SELECTCONNECTIONPROFILEDIALOG_H
#define SELECTCONNECTIONPROFILEDIALOG_H

#include "connectionprofile.h"
#include "data/player.h"
#include <QDialog>
#include <QSettings>

#include <QAbstractListModel>

namespace Ui
{
    class SelectConnectionProfileDialog;
}

/**
 * @brief The ProfileModel class stores all users profile. It is read from settings. User can add, update or remove
 * profile.
 */
class ProfileModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @brief ProfileModel
     */
    ProfileModel(QString version);
    /**
     * @brief ~ProfileModel
     */
    virtual ~ProfileModel();
    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    virtual int rowCount(const QModelIndex& parent) const;
    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    virtual QVariant data(const QModelIndex& index, int role) const;
    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const;
    /**
     * Load informations from the previous rolisteam's execution
     */
    void readSettings();

    /**
     * Save parameters for next executions.
     */
    void writeSettings();

    Qt::ItemFlags flags(const QModelIndex& index) const;
    void removeProfile(ConnectionProfile* profile);

    ConnectionProfile* getProfile(const QModelIndex&);

    void cloneProfile(const QModelIndex& index);

    int indexOf(ConnectionProfile* tmp);
    ConnectionProfile* getProfile(int index);
public slots:
    /**
     * @brief ProfileModel::appendProfile
     */
    void appendProfile();
    /**
     * @brief append profile with param
     * @param profile
     */
    void appendProfile(ConnectionProfile* profile);

private:
    QList<ConnectionProfile*> m_connectionProfileList;
    QString m_version;
};

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
    explicit SelectConnectionProfileDialog(QString version, QWidget* parent= 0);
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
