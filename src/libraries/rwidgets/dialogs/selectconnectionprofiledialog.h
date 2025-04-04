#ifndef SELECTCONNECTIONPROFILEDIALOG_H
#define SELECTCONNECTIONPROFILEDIALOG_H

#include "customs/filedirchooser.h"
#include "data/player.h"
#include "network/connectionprofile.h"

#include "rwidgets_global.h"
#include <QAbstractItemModel>
#include <QDialog>
#include <QDir>
#include <QPointer>
#include <QSettings>
#include <memory>

#include "network/characterdatamodel.h"

namespace Ui
{
class SelectConnectionProfileDialog;
}

class GameController;
class ProfileModel;
class SelectConnProfileController;
/**
 * @brief The SelectConnectionProfileDialog class is the dialog box shown at starting or when the connection is lost.
 */
class RWIDGET_EXPORT SelectConnectionProfileDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SelectConnectionProfileDialog(GameController* ctrl, QWidget* parent= nullptr);
    virtual ~SelectConnectionProfileDialog();
    void setArgumentProfile(QString host, int port, QByteArray password);

public slots:
    QByteArray openImage(const QString& path= QDir::homePath());

private:
    Ui::SelectConnectionProfileDialog* ui;
    QPointer<GameController> m_gameCtrl;
    std::unique_ptr<SelectConnProfileController> m_ctrl;

    QString m_avatarUri;
    int m_currentProfileIndex= -1;
    bool m_passChanged= false;
};

#endif // SELECTCONNECTIONPROFILEDIALOG_H
