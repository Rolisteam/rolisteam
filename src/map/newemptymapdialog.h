/***************************************************************************
 *	Copyright (C) 2014 by Renaud Guezennec                                 *
 *   http://www.rolisteam.org/contact                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef NEWEMPTYMAPDIALOG_H
#define NEWEMPTYMAPDIALOG_H

#include <QDialog>
#include "widgets/colorbutton.h"

#include "map/map.h"
#include <QMessageBox>

namespace Ui {
class NewEmptyMapDialog;
}
/**
 * @brief The NewEmptyMapDialog class is the dialog displayed to the user to select properties of the map he wants.
 */
class NewEmptyMapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEmptyMapDialog(QWidget *parent = 0);
    ~NewEmptyMapDialog();

    Map::PermissionMode getPermission() const;
    QString getTitle( ) const;
    QColor getColor( ) const;
    QSize getSize() const;
    QStringList getPermissionData();

public slots:
    /**
     * @brief updateSize radiobutton given the format choice.
     */
    void updateSize();
    /**
     * @brief resetData- reset data to default
     */
    void resetData();

private:
    Ui::NewEmptyMapDialog *ui;

    QStringList  m_labelModel;
    QStringList  m_permissionData;
    QList<QSize> m_landscapeModel;
    QList<QSize> m_portraitModel;
    QList<QSize> m_squareModel;
    ColorButton* m_colorButton;
};

#endif // NEWEMPTYMAPDIALOG_H
