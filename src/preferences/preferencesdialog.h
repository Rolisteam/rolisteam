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

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QStyledItemDelegate>


#include "preferences/dicealiasmodel.h"
#include "preferences/preferencesmanager.h"
#include "diceparser/diceparser.h"
#include "widgets/colorbutton.h"
#include "widgets/filedirchooser.h"
#include "widgets/centeredcheckbox.h"
#include "preferences/palettemodel.h"
#include "preferences/rolisteamtheme.h"

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
    CheckBoxDelegate(bool aRedCheckBox = false, QObject *parent = 0);
    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    virtual QWidget*	createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;
    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    virtual void	setEditorData(QWidget * editor, const QModelIndex & index) const;
    /**
     * @brief setModelData
     * @param editor
     * @param model
     * @param index
     */
    virtual void	setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    /**
     * @brief sizeHint
     * @param option
     * @param index
     * @return
     */
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

public slots:
    void commitEditor();

private:
    CenteredCheckBox* m_editor;
};

namespace Ui {
class PreferencesDialogBox;
}

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
    PreferencesDialog(QWidget * parent = NULL, Qt::WindowFlags f = 0);
    /**
     * @brief ~PreferencesDialog
     */
    virtual ~PreferencesDialog();

    /**
     * @brief sendOffAllDiceAlias
     */
    void sendOffAllDiceAlias(NetworkLink*);
    /**
     * @brief initializeStyle
     */
    void initializeStyle();

public slots:
    /**
     * @brief show
     */
    void show();

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
	//Management of DiceAliases
    /**
     * @brief managedAction
     */
    void managedAction();
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
    void dupplicateTheme();
    void setTitleAtCurrentTheme();
    void setStyle();
    void editColor(QModelIndex);
    void editCss();
    void exportTheme();
    bool importTheme();

private:
    PreferencesManager* m_preferences;
    Ui::PreferencesDialogBox* ui;
	DiceParser* m_diceParser;
	DiceAliasModel* m_aliasModel;
    QPushButton* m_applyBtn;
    PaletteModel* m_paletteModel;
    QList<RolisteamTheme*> m_themes;
};

#endif
