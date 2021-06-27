#ifndef SELECTCONNECTIONPROFILEDIALOG_H
#define SELECTCONNECTIONPROFILEDIALOG_H

#include "customs/filedirchooser.h"
#include "data/player.h"
#include "network/connectionprofile.h"

#include <QAbstractItemModel>
#include <QDialog>
#include <QPointer>
#include <QSettings>

namespace Ui
{
class SelectConnectionProfileDialog;
}

class GameController;
class CharacterDataModel;
class ProfileModel;
/**
 * @brief The SelectConnectionProfileDialog class is the dialog box shown at starting or when the connection is lost.
 */
class SelectConnectionProfileDialog : public QDialog
{
    Q_OBJECT

public:
    enum class State
    {
        IDLE,
        LOADING,
        LOADED,
        CONNECTING
    };
    /**
     * @brief SelectConnectionProfileDialog
     * @param parent
     */
    explicit SelectConnectionProfileDialog(GameController* ctrl, QWidget* parent= nullptr);
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
    void stopConnecting();
    void connectToIndex(QModelIndex index);
    QByteArray openImage(const QString& path);
    void errorOccurs(QString);
    void checkConnection();
    void selectPlayerAvatar();
    void cloneProfile();

private slots:
    void setState(State);

private:
    Ui::SelectConnectionProfileDialog* ui;
    QPointer<GameController> m_ctrl;
    QPointer<ProfileModel> m_model;
    std::unique_ptr<CharacterDataModel> m_characterModel;
    QString m_avatarUri;
    int m_currentProfileIndex= -1;
    bool m_passChanged= false;
    State m_state= State::IDLE;
};

#endif // SELECTCONNECTIONPROFILEDIALOG_H
