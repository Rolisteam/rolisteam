/***************************************************************************
 *   Copyright (C) 2016 by Renaud Guezennec                                *
 *   http://www.rolisteam.org/contact                                      *
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
#ifndef PERSONDIALOG_H
#define PERSONDIALOG_H

#include <QDialog>
#include <QLineEdit>

#include "common/widgets/colorbutton.h"

namespace Ui
{
    class PersonDialog;
}
/**
 * @brief The PersonDialog class asks for all details about new character.
 */
class PersonDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PersonDialog(QWidget* parent= 0);
    virtual ~PersonDialog();

    QString getName() const;
    QColor getColor() const;
    QString getAvatarUri() const;

public slots:
    int edit(QString title, QString name, QColor color, QString icon);

    void openImage();
    void reset();
    void clickOnBar(QAbstractButton* btn);

protected:
    void setVisible(bool visible);

private:
    Ui::PersonDialog* ui;
    QString m_avatar;
};

#endif // PERSONDIALOG_H
