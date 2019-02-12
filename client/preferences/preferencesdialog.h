/*************************************************************************
 *     Copyright (C) 2011 by Renaud Guezennec                            *
 *                                                                       *
 *     http://www.rolisteam.org/                                         *
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
#include <QStyledItemDelegate>

#include "common/widgets/colorbutton.h"
#include "diceparser/diceparser.h"
#include "preferences/characterstatemodel.h"
#include "preferences/dicealiasmodel.h"
#include "preferences/palettemodel.h"
#include "preferences/preferencesmanager.h"
#include "preferences/rolisteamtheme.h"
#include "widgets/centeredcheckbox.h"
#include "widgets/filedirchooser.h"

/**
 * @brief The CheckBoxDelegate class
 */
class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /**
     * @brief CheckBoxDelegate
     * @param aRedCheckBox
     * @param parent
     */
    CheckBoxDelegate(bool aRedCheckBox= false, QObject* parent= nullptr);
    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    /**
     * @brief setModelData
     * @param editor
     * @param model
     * @param index
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;
    /**
     * @brief sizeHint
     * @param option
     * @param index
     * @return
     */
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

public slots:
    void commitEditor();

private:
    CenteredCheckBox* m_editor= nullptr;
};

/**
 * @brief The ColorListEditor class
 */
class ColorListEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true)

public:
    ColorListEditor(QWidget* widget= nullptr);

    QColor color() const;
    void setColor(QColor c);

signals:
    void colorChanged();

private:
    void populateList();
};

namespace Ui
{
    class PreferencesDialogBox;
}
/**
 * @brief The ColorDelegate class
 */
class ColorDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /**
     * @brief CheckBoxDelegate
     * @param aRedCheckBox
     * @param parent
     */
    ColorDelegate(QObject* parent= nullptr);
    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    /**
     * @brief setModelData
     * @param editor
     * @param model
     * @param index
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

private:
    // ColorListEditor* m_editor;
};
/**
 * @brief Actually only to change directories.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief PreferencesDialog
     * @param parent
     * @param f
     */
    PreferencesDialog(QWidget* parent= nullptr, Qt::WindowFlags f= 0);
    /**
     * @brief ~PreferencesDialog
     */
    virtual ~PreferencesDialog();

    /**
     * @brief sendOffAllDiceAlias
     */
    void sendOffAllDiceAlias();
    void sendOffAllState();
    /**
     * @brief initializePostSettings loads every data from the settings file.
     */
    void initializePostSettings();

    CharacterStateModel* getStateModel() const;
    void setStateModel(CharacterStateModel* stateModel);

public slots:
    /**
     * @brief show the dialog.
     */
    void show();
    /**
     * @brief updateUi
     * @param isGM
     */
    void updateUi(bool isGM);
    /**
     * @brief manageMessagingPref receives all notification when GUI widget has changed.
     */
    void manageMessagingPref();
    void backgroundChanged();
    void manageHeartBeat();
private slots:
    /**
     * @brief load
     */
    void load();
    /**
     * @brief save
     */
    void save() const;

    /**
     * @brief performDiag start diagnostic and Display some value about current qt version.
     */
    void performDiag();
    // Management of DiceAliases
    /**
     * @brief managedAction
     */
    void manageAliasAction();
    /**
     * @brief manageStateAction
     */
    void manageStateAction();
    /**
     * @brief testAliasCommand
     */
    void testAliasCommand();
    /**
     * @brief applyBackground
     */
    void applyBackground();
    /**
     * @brief updateTheme
     */
    void updateTheme();
    /**
     * @brief dupplicateTheme
     */
    void dupplicateTheme(bool selectNew= true);
    /**
     * @brief setTitleAtCurrentTheme
     */
    void setTitleAtCurrentTheme();
    /**
     * @brief setStyle
     */
    void setStyle();
    /**
     * @brief editColor
     */
    void editColor(QModelIndex);
    /**
     * @brief editCss
     */
    void editCss();
    /**
     * @brief exportTheme
     */
    void exportTheme();
    /**
     * @brief importTheme
     * @return
     */
    bool importTheme();
    /**
     * @brief deleteTheme
     */
    void deleteTheme();

private:
    /**
     *  @brief getCurrentRemovableTheme should return the current theme which can be modified.
     */
    RolisteamTheme* getCurrentRemovableTheme(bool selectNew= true);

private:
    PreferencesManager* m_preferences= nullptr;
    Ui::PreferencesDialogBox* ui;
    DiceParser* m_diceParser;
    DiceAliasModel* m_aliasModel;
    QPushButton* m_applyBtn;
    PaletteModel* m_paletteModel;
    QList<RolisteamTheme*> m_themes;
    CharacterStateModel* m_stateModel;
    bool m_currentThemeIsEditable;
};

#endif
