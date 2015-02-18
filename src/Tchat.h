/*************************************************************************
 *   Copyright (C) 2007 by Romain Campioni                               *
 *   Copyright (C) 2009 by Renaud Guezennec                              *
 *   Copyright (C) 2011 by Joseph Boudou                                 *
 *                                                                       *
 *   http://www.rolisteam.org/                                           *
 *                                                                       *
 *   Rolisteam is free software; you can redistribute it and/or modify   *
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


#ifndef TCHAT_H
#define TCHAT_H

#include <QSplitter>
#include <QFile>
#include <QTextEdit>

#include "types.h"

class Liaison;
class MainWindow;
class Player;
class TextEditAmeliore;


/**
 * @brief Fenêtre permettant aux utilisateurs de communiquer entre eux par
 * écrit.
 */    
class Tchat : public QSplitter
{
Q_OBJECT

public :
    Tchat(const QString & id, const QString & name, MainWindow * parent = NULL);
    Tchat(Player * player, MainWindow * parent = NULL);
    ~Tchat();

    QString identifiant() const;
    QString name() const;

    QAction * toggleViewAction() const;

    void afficherMessage(const QString &utilisateur, const QColor &couleur, const QString &message, actionDiscussion msgtype = TCHAT_MESSAGE);

    bool hasUnseenMessage() const;

    // events handler
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void changed(Tchat * what);

public slots:
    virtual void setVisible(bool visible);
    void save();

protected :
    void init(MainWindow * parent);

    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);

    /**
     * @brief read and remove a number from a string.
     * \param[in/out] string from wich the number will be read and removed
     * \param[out] value of the readed number
     * \return true if no problem occured
     */
    bool GetNumber(QString &str, int* value);

private :
    static QStringList m_keyWordList;

    QString m_uuid;
    QString m_name;
    QString m_filename;
    Liaison * m_link;
    bool m_warnedEmoteUnavailable;
    bool m_hasUnseenMessage;

    QTextEdit * zoneAffichage;         // Zone de texte ou s'affichent les messages
    TextEditAmeliore * zoneEdition;    // Zone de texte ou l'utilisateur peut ecrire
    QAction * m_toggleViewAction;

    QList<QString> historiqueMessages; // Contient l'historique des anciens messages
    int numHistorique;                 // Numero de la ligne de l'historique actuelle

    int calculerJetDes(QString &message, QString &tirage, bool &ok);
    int calculerJetDesSR4(QString &message, QString &tirage, QString &glitch, bool ok);

private slots :
    void emettreTexte();
    void monterHistorique();
    void descendreHistorique();
};

#endif
