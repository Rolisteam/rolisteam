#ifndef CONNECTIONPROFILEMODEL_H
#define CONNECTIONPROFILEMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <memory>

class ConnectionProfile;
class ProfileModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @brief ProfileModel
     */
    ProfileModel();
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

    Qt::ItemFlags flags(const QModelIndex& index) const;
    void removeProfile(int idx);

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
    std::vector<std::unique_ptr<ConnectionProfile>> m_connectionProfileList;
};

#endif // CONNECTIONPROFILEMODEL_H
