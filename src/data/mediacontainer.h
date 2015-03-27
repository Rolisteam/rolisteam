#ifndef MEDIACONTAINER_H
#define MEDIACONTAINER_H



#include <QString>


#include "data/cleveruri.h"

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

	/**
	 * @brief setCleverUri
	 * @param uri
	 */
	virtual void setCleverUri(CleverURI* uri);

	/**
	 * @brief readFile
	 * @return
	 */
	virtual bool readFileFromUri()=0;


	/**
	 * @brief openFile
	 * @return
	 */
	virtual void openMedia();

	/**
	 * @brief getTitle
	 * @return
	 */
	virtual QString getTitle() const;



protected:
    QString m_localPlayerId;
	CleverURI* m_uri;
	QString m_title;


};

#endif // MEDIACONTAINER_H
