/***************************************************************************
 *	Copyright (C) 2019 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
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
#ifndef IOHELPER_H
#define IOHELPER_H

#include <QJsonDocument>
#include <QString>
#include <QVariant>
#include <map>

class VMap;
class CleverURI;
class VectorialMapController;
class MediaControllerBase;
class ImageController;
class WebpageController;
class NoteController;
class PdfController;
class SharedNoteController;
class CharacterSheetController;
class ContentController;
class MediaManagerBase;
class ContentModel;
class MindMapController;
class IOHelper
{
public:
    IOHelper();

    static QByteArray loadFile(const QString& file);
    static QString readTextFile(const QString& file);
    static bool loadToken(const QString& filename, std::map<QString, QVariant>& params);

    static QByteArray saveController(MediaControllerBase* media);
    static MediaControllerBase* loadController(QDataStream& input);

    static QJsonObject byteArrayToJsonObj(const QByteArray& data);
    static QJsonArray byteArrayToJsonArray(const QByteArray& data);

    static void readCharacterSheetController(CharacterSheetController* ctrl, const QByteArray& array);
    static void readVectorialMapController(VectorialMapController* ctrl, const QByteArray& array);

#ifdef WITH_PDF
    static void readPdfController(PdfController* ctrl, const QByteArray& array);
#endif
    static void readImageController(ImageController* ctrl, const QByteArray& array);
    static void readNoteController(NoteController* ctrl, const QByteArray& array);
    static void readSharedNoteController(SharedNoteController* ctrl, const QByteArray& array);
    static void readWebpageController(WebpageController* ctrl, const QByteArray& array);
    static void readMindmapController(MindMapController* ctrl, const QByteArray& array);
};

#endif // IOHELPER_H
