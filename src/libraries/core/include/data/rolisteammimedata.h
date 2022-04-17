#ifndef ROLISTEAMMIMEDATA_H
#define ROLISTEAMMIMEDATA_H

#include <QMimeData>

#include "data/diceshortcut.h"
#include "data/person.h"
#include <core_global.h>
/**
 * @brief allows drag and drop inside the application scope.
 */
class  CORE_EXPORT RolisteamMimeData : public QMimeData
{
    Q_OBJECT
    Q_PROPERTY(Person* person READ person WRITE setPerson NOTIFY personChanged)
    Q_PROPERTY(DiceShortCut alias READ alias NOTIFY aliasChanged)
    Q_PROPERTY(QString npcUuid READ npcUuid WRITE setNpcUuid NOTIFY npcUuidChanged)
    Q_PROPERTY(QString mediaUuid READ mediaUuid WRITE setMediaUuid NOTIFY mediaUuidChanged)
    // Q_PROPERTY(QByteArray imageData READ imageData WRITE setImageData NOTIFY imageDataChanged)
public:
    RolisteamMimeData();
    bool hasFormat(const QString& mimeType) const;

    Person* person() const;
    DiceShortCut alias() const;
    QString npcUuid() const;
    QString mediaUuid() const;
    // QByteArray imageData() const;

public slots:
    void setAlias(QString, QString, bool);
    void setPerson(Person*);
    // void setImageData(const QByteArray& data);
    void setNpcUuid(const QString& str);
    void setMediaUuid(const QString& uuid);

signals:
    void personChanged();
    void aliasChanged();
    void npcUuidChanged();
    void mediaUuidChanged();
    // void imageDataChanged();

private:
    Person* m_person= nullptr; /// pointer to stored data
    DiceShortCut m_alias;      /// alias
    // QByteArray m_imageData;
    QString m_npcUuid;
    QString m_mediaUuid;
};
#endif // ROLISTEAMMIMEDATA_H
