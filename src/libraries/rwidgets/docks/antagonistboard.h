#ifndef CAMPAIGN_ANTAGONIST_BOARD_H
#define CAMPAIGN_ANTAGONIST_BOARD_H

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
class AntagonistBoard : public QWidget
{
    Q_OBJECT

public:
    explicit AntagonistBoard(campaign::CampaignEditor* editor, QWidget* parent= nullptr);
    ~AntagonistBoard();

protected slots:
    /*void importNpc();
     void exportNpc();*/

    void updateImage(const QByteArray& data);
    void contextualMenu(const QPoint& pos);

    void hideColumn();

private:
    Ui::AntagonistBoard* ui;
    std::unique_ptr<AntagonistBoardController> m_ctrl;

    std::unique_ptr<QAction> m_createTokenAct;
    std::unique_ptr<QAction> m_cloneCharacterAct;
    std::unique_ptr<QAction> m_changeImageAct;

    QString m_currentItemId;
    std::vector<std::unique_ptr<QAction>> m_columnsAction;
    std::unique_ptr<QAction> m_fullModeAct;
};
} // namespace campaign
#endif // CAMPAIGN_ANTAGONIST_BOARD_H
