#ifndef MEDIACONTAINER_H
#define MEDIACONTAINER_H

#include <QString>
/**
 * @brief The MediaContainer class
 */
class MediaContainer
{
public:
    /**
     * @brief MediaContainer
     */
    MediaContainer();

    /**
     * @brief setLocalPlayerId
     * @param id
     */
    void setLocalPlayerId(QString id);
    /**
     * @brief getLocalPlayerId
     * @return
     */
    QString getLocalPlayerId();
protected:
    QString m_localPlayerId;


};

#endif // MEDIACONTAINER_H
