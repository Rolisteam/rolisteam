#ifndef SELECTCONNECTIONPROFILEDIALOG_H
#define SELECTCONNECTIONPROFILEDIALOG_H

#include "connectionprofile.h"
#include "data/player.h"
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
    void connectToIndex(QModelIndex index);
    QString openImage(const QString& path);
    void errorOccurs(QString);
    void checkConnection();

private:
    Ui::SelectConnectionProfileDialog* ui;
    QPointer<GameController> m_ctrl;
    QPointer<ProfileModel> m_model;
    std::unique_ptr<CharacterDataModel> m_characterModel;
    QString m_avatarUri;
    int m_currentProfileIndex= -1;
    bool m_passChanged= false;
};

#endif // SELECTCONNECTIONPROFILEDIALOG_H
