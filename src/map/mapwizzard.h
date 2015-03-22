#ifndef MAPWIZZARD_H
#define MAPWIZZARD_H

#include <QDialog>



#include "preferences/preferencesmanager.h"
#include "map/map.h"

namespace Ui {
class MapWizzard;
}
/**
 * @brief The MapWizzard class
 */
class MapWizzard : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief MapWizzard
     * @param parent
     */
    explicit MapWizzard(QWidget *parent = 0);
    /**
      * @brief ~MapWizzard destructor
      */
    ~MapWizzard();
    /**
     * @brief getPermissionMode
     * @return
     */
    Map::PermissionMode getPermissionMode() const;
    /**
     * @brief getFilepath
     * @return
     */
    QString getFilepath() const;
    /**
     * @brief getHidden
     * @return
     */
    bool getHidden() const;
    /**
     * @brief getTitle
     * @return
     */
    QString getTitle() const;
    /**
     * @brief resetData
     */
    void resetData();

private slots:
    /**
     * @brief selectPath
     */
    void selectPath();

private:
    /**
     * @brief ui
     */
    Ui::MapWizzard* ui;
    /**
     * @brief m_preferences
     */
    PreferencesManager* m_preferences;
};

#endif // MAPWIZZARD_H
