/*************************************************************************
 *     Copyright (C) 2011 by Renaud Guezennec                            *
 *                                                                       *
 *     https://rolisteam.org/                                         *
 *                                                                       *
 *   rolisteam is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published   *
 *   by the Free Software Foundation; either version 2 of the License,   *
 *   or (at your option) any later version.                              *
 *                                                                       *
 *   This program is distributed in the hope that it will be useful,     *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of      *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
 *   GNU General Public License for more details.                        *
 *                                                                       *
 *   You should have received a copy of the GNU General Public License   *
 *   along with this program; if not, write to the                       *
 *   Free Software Foundation, Inc.,                                     *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           *
 *************************************************************************/

#ifndef PREFERENCES_DIALOG_H
#define PREFERENCES_DIALOG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QStringListModel>
#include <QStyledItemDelegate>

#include "common_widgets/colorbutton.h"
#include "data/rolisteamtheme.h"
#include "model/dicealiasmodel.h"
#include "model/palettemodel.h"
#include "preferences/preferencesmanager.h"
#include "rwidgets/customs/centeredcheckbox.h"
// #include "widgets/filedirchooser.h"
#include "rwidgets_global.h"
class PreferencesController;

namespace Ui
{
class PreferencesDialogBox;
}

/**
 * @brief Actually only to change directories.
 */
class RWIDGET_EXPORT PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    enum class PreferenceTab
    {
        General,
        Player,
        Themes,
        Diagnostic
    };
    Q_ENUM(PreferenceTab)
    enum AudioPlayerDataIndex
    {
        First,
        Second,
        Third
    };
    Q_ENUM(AudioPlayerDataIndex)

    PreferencesDialog(PreferencesController* controller, QWidget* parent= nullptr, Qt::WindowFlags f= Qt::Dialog);
    virtual ~PreferencesDialog();

public slots:
    void show(PreferenceTab tab= PreferenceTab::General);
    void backgroundChanged();
    void manageHeartBeat();
    void manageMessagingPref();
    void updateTranslationPref();

private slots:
    void load();
    void save() const;
    void performDiag();
    void testAliasCommand();
    void updateTheme();
    void dupplicateTheme(bool selectNew= true);
    void setTitleAtCurrentTheme();
    void setStyle();
    void editColor(const QModelIndex &);
    void editCss();
    void exportTheme();
    bool importTheme();
    void deleteTheme();

private:
    QPointer<PreferencesManager> m_preferences;
    Ui::PreferencesDialogBox* ui;

    QPushButton* m_applyBtn;
    QPointer<PreferencesController> m_ctrl;
    bool m_currentThemeIsEditable;
};

#endif
