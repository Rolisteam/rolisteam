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
    Q_PROPERTY(bool rcseAvailable READ rcseAvailable WRITE setRcseAvailable NOTIFY rcseAvailableChanged FINAL)
    Q_PROPERTY(
        bool mindmapAvailable READ mindmapAvailable WRITE setMindmapAvailable NOTIFY mindmapAvailableChanged FINAL)
    Q_PROPERTY(bool textEditorAvailable READ textEditorAvailable WRITE setTextEditorAvailable NOTIFY
                   textEditorAvailableChanged FINAL)
public:
    explicit CampaignView(QWidget* parent= nullptr);
    QModelIndexList getSelection() const;

    QString currentChapter() const;

    bool textEditorAvailable() const;
    void setTextEditorAvailable(bool newTextEditorAvailable);

    bool mindmapAvailable() const;
    void setMindmapAvailable(bool newMindmapAvailable);

    bool rcseAvailable() const;
    void setRcseAvailable(bool newRcseAvailable);

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
    void openExternally(const QString& id, const QString& path, Core::ContentType type);
    void currentChapterChanged();

    void textEditorAvailableChanged();

    void mindmapAvailableChanged();

    void rcseAvailableChanged();

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
    QAction* m_renameAct;

    QAction* m_nameColsAct;
    QAction* m_sizeColsAct;
    QAction* m_typeColsAct;
    QAction* m_addedColsAct;
    QAction* m_modifiedColsAct;

    QAction* m_editExternallyAct;

    QHash<Core::MediaType, QList<QAction*>> m_convertionHash;
    QModelIndex m_index;
    bool m_textEditorAvailable{false};
    bool m_mindmapAvailable{false};
    bool m_rcseAvailable{false};
};
} // namespace campaign
#endif // SESSIONVIEW_H
