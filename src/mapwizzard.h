#ifndef MAPWIZZARD_H
#define MAPWIZZARD_H

#include <QDialog>



#include "preferencesmanager.h"
#include "Carte.h"

namespace Ui {
class MapWizzard;
}

class MapWizzard : public QDialog
{
    Q_OBJECT

public:
    explicit MapWizzard(QWidget *parent = 0);
    ~MapWizzard();

    Carte::PermissionMode getPermissionMode() const;
    QString getFilepath() const;
    bool getHidden() const;
    QString getTitle() const;

    void resetData();

private slots:
    void selectPath();

private:
    Ui::MapWizzard* ui;
    PreferencesManager* m_preferences;
};

#endif // MAPWIZZARD_H
