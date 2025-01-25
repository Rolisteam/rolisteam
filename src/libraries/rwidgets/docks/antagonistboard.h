#ifndef CAMPAIGN_ANTAGONIST_BOARD_H
#define CAMPAIGN_ANTAGONIST_BOARD_H

#include "rwidgets_global.h"
#include <QPointer>
#include <QWidget>
#include <memory>

namespace Ui
{
class AntagonistBoard;
}
class Character;

namespace campaign
{
class CampaignEditor;
class AntagonistBoardController;
class RWIDGET_EXPORT AntagonistBoard : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool fullMode READ fullMode WRITE setFullMode NOTIFY fullModeChanged)
    Q_PROPERTY(bool minimalMode READ minimalMode WRITE setMinimalMode NOTIFY minimalModeChanged)
public:
    explicit AntagonistBoard(campaign::CampaignEditor* editor, QWidget* parent= nullptr);
    ~AntagonistBoard();

    const std::vector<std::unique_ptr<QAction>>& columnsActions() const;

    bool fullMode() const;
    void setFullMode(bool newFullMode);

    bool minimalMode() const;
    void setMinimalMode(bool newMinimalMode);

signals:
    void fullModeChanged();
    void minimalModeChanged();

protected slots:
    void updateImage(const QByteArray& data);
    void contextualMenu(const QPoint& pos);

    void hideColumn();

    virtual bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Ui::AntagonistBoard* ui;
    QPointer<campaign::CampaignEditor> m_editor;
    std::unique_ptr<AntagonistBoardController> m_ctrl;

    std::unique_ptr<QAction> m_createTokenAct;
    std::unique_ptr<QAction> m_cloneCharacterAct;
    std::unique_ptr<QAction> m_changeImageAct;
    std::unique_ptr<QAction> m_fullModeAct;
    std::unique_ptr<QAction> m_minimalModeAct;
    std::unique_ptr<QAction> m_saveTokenAct;

    QString m_currentItemId;
    std::vector<std::unique_ptr<QAction>> m_columnsAction;
};
} // namespace campaign
#endif // CAMPAIGN_ANTAGONIST_BOARD_H
