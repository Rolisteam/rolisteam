/*
    Cahoots is a crossplatform real-time collaborative text editor.

    Copyright (C) 2010 Chris Dimpfl, Anandi Hira, David Vega

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "utilities.h"

#include <QString>

// Fonts
 QString Utilities::s_codeFont;// = getSystem() == "Windows" ? "Courier New" : (getSystem() == "Mac" ? "Monaco" : "Courier 10 Pitch");
 int Utilities::s_codeFontSize= 8;// = getSystem() == "Mac" ? 11 : 9;
 QString Utilities::s_labelFont;// = getSystem() == "Mac" ? "Lucida Grande" : "Verdana";
 int Utilities::s_labelFontSize = 8;// = getSystem() == "Mac" ? 11 : 8;
 QString Utilities::s_chatFont;// = getSystem() == "Mac" ? "Lucida Grande" : "Verdana";
 int Utilities::s_chatFontSize= 8;// = getSystem() == "Mac" ? 11 : 8;

Utilities::Utilities()
{
    getSystem();
}

QString Utilities::getSystem()
{
    s_codeFont = QStringLiteral("Courier 10 Pitch");
    s_codeFontSize = 9;
    s_labelFont =  QStringLiteral("Verdana");
    s_labelFontSize = 8;
    s_chatFont = QStringLiteral("Verdana");
    s_chatFontSize = 8;
    return QStringLiteral("Windows");
}

