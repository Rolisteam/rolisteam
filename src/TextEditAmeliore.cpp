/***************************************************************************
 *	Copyright (C) 2007 by Romain Campioni   			   *
 *	Copyright (C) 2010 by Renaud Guezennec                             *
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


	#include <QtGui>

	#include "TextEditAmeliore.h"


	/********************************************************************/
	/* Constructeur                                                     */
	/********************************************************************/	
    TextEditAmeliore::TextEditAmeliore(QWidget *parent)
        : QTextEdit(parent)
    {
	}
	
	/********************************************************************/	
	/* Verifie si l'utilisateur a appuye sur la touche Entree. Si oui   */
	/* le signal entreePressee est emis                                 */
	/********************************************************************/	
	void TextEditAmeliore::keyPressEvent(QKeyEvent *e)
	{
		// On recupere la touche qui vient d'etre enfoncee
		int touche = e->key();
		
		// S'il s'agit de la touche Entree ou Return, on emet le signal entreePressee
		if (touche == Qt::Key_Return || touche == Qt::Key_Enter)
			emit entreePressee();

		// S'il s'agit de la touche fleche haut, on emet le signal hautPressee
		else if (touche == Qt::Key_Up)
			emit hautPressee();

		// S'il s'agit de la touche fleche bas, on emet le signal basPressee
		else if (touche == Qt::Key_Down)
			emit basPressee();

		// Sinon on passe l'evenement au QTextEdit
		else
			QTextEdit::keyPressEvent(e);
	}
