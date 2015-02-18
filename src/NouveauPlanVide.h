/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2009 by Renaud Guezennec                             *
 *   http://renaudguezennec.homelinux.org/accueil,3.html                   *
 *                                                                         *
 *   rolisteam is free software; you can redistribute it and/or modify  *
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


/********************************************************************/
/*                                                                  */
/* Fenetre destinee a parametrer un nouveau plan vide avant sa      */
/* creation                                                         */
/*                                                                  */
/********************************************************************/	


#ifndef NOUVEAU_PLAN_VIDE_H
#define NOUVEAU_PLAN_VIDE_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QComboBox>

class MainWindow;
/**
 * @brief The NouveauPlanVide class
 */
class NouveauPlanVide : public QDialog
{
    Q_OBJECT

public :
    enum PermissionMode{GM_ONLY, PC_MOVE,PC_ALL };

    NouveauPlanVide(MainWindow* mainWindow,QWidget *parent = 0);


signals :
        void creerNouveauPlanVide(QString titre, QString idCarte, QColor couleurFond, quint16 largeur, quint16 hauteur,quint8);

protected:
        NouveauPlanVide::PermissionMode getPermission(int id);

private slots :
        void ouvrirSelecteurCouleur();
        void changementFormat(int bouton);
        void changementDimensions(int bouton);
        void validerDimensions();


private :
        void majIntitulesBoutons();
        QGroupBox *groupeFormat;
        QGroupBox *groupeTaille;
        QLineEdit *titrePlan;
        QLineEdit *largeurPlan;
        QLineEdit *hauteurPlan;
        QPushButton *couleurFond;
        QRadioButton *boutonPetitPlan;
        QRadioButton *boutonMoyenPlan;
        QRadioButton *boutonGrandPlan;
        QRadioButton *boutonTresGrandPlan;
        QRadioButton *boutonPersonnalise;
        QWidget *taillePersonnalisee;
        QComboBox* m_permissionMode;
        MainWindow* m_mainWindow;


        int format;							// Indice dans les tableaux ci-dessous correspondant au format selectionne
        int dimensions;						// Indice dans les tableaux ci-dessous correspondant aux dimensions selectionnees

        // Tableaux contenant les dimensions du plan au format [format] [dimension]
        static quint16 largeur[3][4];
        static quint16 hauteur[3][4];

};

#endif
