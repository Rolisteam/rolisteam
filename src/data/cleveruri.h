#ifndef CLEVERURI_H
#define CLEVERURI_H

#include <QString>
#include <QMetaType>
#include "ressourcesnode.h"

class CleverURI : public RessourcesNode
{

public:
    enum ContentType {MAP,TCHAT,PICTURE,TEXT,CHARACTERSHEET,SCENARIO};
    CleverURI();
    CleverURI(const CleverURI & mp);
    CleverURI(QString uri,ContentType type);
    ~CleverURI();

    void setUri(QString& uri);
    void setType(int type);

    const QString& getUri() const;
    int getType() const;
    bool operator==(const CleverURI& uri1) const;

    bool hasChildren() const;
    const QString& getShortName() const;
    virtual void setShortName(QString& name);

    static QString& getIcon(ContentType type);

private:
    void defineShortName();




    QString m_uri;
    int m_type;
    QString m_shortname;


    static QString m_textIcon;
    static QString m_mapIcon;
    static QString m_pictureIcon;
    static QString m_charactersheetIcon;
    static QString m_scenarioIcon;
    static QString m_tchatIcon;

    friend QDataStream& operator<<(QDataStream& os,const CleverURI&);
    friend QDataStream& operator>>(QDataStream& is,CleverURI&);
};
typedef QList<CleverURI> CleverUriList;
Q_DECLARE_METATYPE(CleverURI)
Q_DECLARE_METATYPE(CleverUriList)

#endif // CLEVERURI_H
