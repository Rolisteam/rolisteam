#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H
#include <QString>
#include <QTcpSocket>
#include <QNetworkAccessManager>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    UpdateChecker();

    bool mustBeUpdated();
    void startChecking();

    QString& getLatestVersion();
    QString& getLatestVersionDate();
signals:
    void checkFinished();

private slots:
    void readXML(QNetworkReply* p);

private:
    QString m_version;
    QString m_versionDate;
    QTcpSocket m_socket;
    QNetworkAccessManager* m_manager;
    bool m_state;
};

#endif // UPDATECHECKER_H
