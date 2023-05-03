#ifndef SESSIONVIEW_H
#define SESSIONVIEW_H

#include <QDrag>
#include <QHash>
#include <QPointer>
#include <QTreeView>

#include "media/mediatype.h"
#include "rwidgets_global.h"

namespace campaign
{
class Campaign;
class MediaNode;
class RWIDGET_EXPORT CampaignView : public QTreeView
{
    Q_OBJECT
    Q_PROPERTY(QString currentChapter READ currentChapter NOTIFY currentChapterChanged)
public:
    explicit CampaignView(QWidget* parent= nullptr);
    QModelIndexList getSelection() const;

    QString currentChapter() const;

public slots:
    void setCampaign(Campaign* campaign);

protected:
    void startDrag(Qt::DropActions supportedActions);
    void contextMenuEvent(QContextMenuEvent* event);
    void mouseDoubleClickEvent(QMouseEvent*);

signals:
    void addDirectory(QModelIndex&);
    void removeSelection(const QString& uuid);
    void openAs(const QString& id, const QString& path, Core::ContentType type);
    void currentChapterChanged();

private slots:
    void setCurrentChapter(const QString& chapter);
    void onAddChapter();

    void addOpenAsActs(QMenu* menu, Core::MediaType type);

private:
    QPointer<Campaign> m_campaign;

    QAction* m_addDirectoryAct;
    QAction* m_deleteFileAct;
    QAction* m_defineAsCurrent;
    QAction* m_openAct;
    QAction* m_openAsAct;
    QAction* m_renameAct;

    QAction* m_nameColsAct;
    QAction* m_sizeColsAct;
    QAction* m_typeColsAct;
    QAction* m_addedColsAct;
    QAction* m_modifiedColsAct;

    QHash<Core::MediaType, QList<QAction*>> m_convertionHash;
    QModelIndex m_index;
};
} // namespace campaign
#endif // SESSIONVIEW_H
