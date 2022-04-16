/***************************************************************************
 *	Copyright (C) 2021 by Renaud Guezennec                               *
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
#include "utilshelper.h"

#include <QBrush>
#include <QImageReader>
#include <QPainter>
#include <QtConcurrent>

namespace helper
{
namespace utils
{
QString allSupportedImageFormatFilter()
{
    auto allFormats= QImageReader::supportedImageFormats();

    QStringList list;
    std::transform(std::begin(allFormats), std::end(allFormats), std::back_inserter(list),
                   [](const QByteArray& array) { return QString("*.%1").arg(QString::fromLocal8Bit(array)); });

    return list.join(" ");
}

QRectF computerBiggerRectInside(const QRect& rect, qreal ratio)
{
    QRectF res(rect);
    auto const r= ratio;

    auto const reversed= 1 / r;

    auto future_W= res.height() * r;
    auto const future_H= res.width() * reversed;

    if(future_W < future_H && future_W <= rect.width())
    {
        res.setWidth(future_W);
    }
    else
    {
        if(future_H <= res.height())
            res.setHeight(future_H);
        else
        {
            res.setWidth(future_W);
        }
    }

    return res;
}

QPixmap roundCornerImage(const QPixmap& source, int size, int radius)
{
    QPixmap img(size, size);
    img.fill(Qt::transparent);
    QPainter painter(&img);
    QBrush brush;
    brush.setTextureImage(source.toImage().scaled(size, size));
    painter.setBrush(brush);
    painter.drawRoundedRect(0, 0, size, size, radius, radius);
    return img;
}

bool isSquareImage(const QByteArray& array)
{
    auto img= QImage::fromData(array);
    return !img.isNull() && img.width() == img.height();
}

bool hasValidCharacter(const std::vector<connection::CharacterData>& characters, bool isGameMaster)
{
    auto res= false;
    if(isGameMaster)
    {
        res= true; // characters.empty();
    }
    else
    {
        res= std::any_of(std::begin(characters), std::end(characters), [](const connection::CharacterData& data) {
            return !data.m_name.isEmpty() && isSquareImage(data.m_avatarData);
        });
    }
    return res;
}

QStringList extentionPerType(Core::ContentType type, bool save, bool wildcard)
{
    QStringList exts;
    switch(type)
    {
    case Core::ContentType::CHARACTERSHEET:
        exts= QStringList{Core::extentions::EXT_SHEET}; // QObject::tr("Character Sheets files  (%1)").arg("*.rcs");
        break;
    case Core::ContentType::PICTURE:
        exts= QStringList{Core::extentions::EXT_IMG_BMP, Core::extentions::EXT_IMG_JPG, Core::extentions::EXT_IMG_JPEG,
                          Core::extentions::EXT_IMG_PNG, Core::extentions::EXT_IMG_SVG, Core::extentions::EXT_IMG_GIF};
        break;
    case Core::ContentType::NOTES:
        exts= save ? QStringList{Core::extentions::EXT_OPEN_DOCUMENT} :
                     QStringList{Core::extentions::EXT_OPEN_DOCUMENT, Core::extentions::EXT_HTM,
                                 Core::extentions::EXT_HTML,          Core::extentions::EXT_XHTML,
                                 Core::extentions::EXT_MARKDOWN,      Core::extentions::EXT_TEXT};

        break;
    case Core::ContentType::SHAREDNOTE:
        exts= save ? QStringList{Core::extentions::EXT_SHAREDNOTE} :
                     QStringList{Core::extentions::EXT_SHAREDNOTE, Core::extentions::EXT_HTM,
                                 Core::extentions::EXT_HTML,       Core::extentions::EXT_XHTML,
                                 Core::extentions::EXT_MARKDOWN,   Core::extentions::EXT_TEXT};

        break;
    case Core::ContentType::WEBVIEW:
        exts= save ? QStringList{Core::extentions::EXT_WEBVIEW} :
                     QStringList{
                         Core::extentions::EXT_WEBVIEW,
                         Core::extentions::EXT_HTM,
                         Core::extentions::EXT_HTML,
                         Core::extentions::EXT_XHTML,
                     };
        break;
#ifdef WITH_PDF
    case Core::ContentType::PDF:
        exts= QStringList{Core::extentions::EXT_PDF}; // QObject::tr("Pdf File (%1)").arg("*.pdf");
        break;
#endif
    case Core::ContentType::VECTORIALMAP:
        exts= QStringList{Core::extentions::EXT_MAP}; // QObject::tr("Vectorial Map (%1)").arg("*.vmap");
        break;
    case Core::ContentType::MINDMAP:
        exts= QStringList{Core::extentions::EXT_MINDMAP}; // QObject::tr("Mindmap (%1)").arg("*.rmap");
        break;
    default:
        break;
    }

    QStringList res;
    if(wildcard)
    {
        res.reserve(exts.size());
        std::transform(std::begin(exts), std::end(exts), std::back_inserter(res),
                       [](const QString& ext) { return QString("*%1").arg(ext); });
    }
    else
    {
        res= exts;
    }
    return res;
}

QString filterForType(Core::ContentType type, bool save)
{
    QString filterType;
    auto list= extentionPerType(type, save, true).join(' ');
    switch(type)
    {
    case Core::ContentType::CHARACTERSHEET:
        filterType= QObject::tr("Character Sheets files  (%1)").arg(list);
        break;
    case Core::ContentType::PICTURE:
        filterType= QObject::tr("Supported Image formats (%1)").arg(list);
        break;
    case Core::ContentType::NOTES:
        filterType= QObject::tr("Supported Text Files (%1)").arg(list);
        break;
    case Core::ContentType::SHAREDNOTE:
        filterType= QObject::tr("Supported Shared Note formats (%1)").arg(list);
        break;
    case Core::ContentType::WEBVIEW:
        filterType= QObject::tr("Supported WebPage (%1)").arg(list);
        break;
#ifdef WITH_PDF
    case Core::ContentType::PDF:
        filterType= QObject::tr("Pdf File (%1)").arg(list);
        break;
#endif
    case Core::ContentType::VECTORIALMAP:
        filterType= QObject::tr("Vectorial Map (%1)").arg(list);
        break;
    case Core::ContentType::MINDMAP:
        filterType= QObject::tr("Mindmap (%1)").arg(list);
        break;
    default:
        filterType= QString();
        break;
    }
    return filterType;
}

QString typeToString(Core::ContentType type)
{
    // WARNING Test with different langs
    QHash<Core::ContentType, QString> names;

    names.insert(Core::ContentType::VECTORIALMAP, QObject::tr("Vectorial Map"));
    names.insert(Core::ContentType::PICTURE, QObject::tr("Picture"));
    names.insert(Core::ContentType::NOTES, QObject::tr("Minutes"));
    names.insert(Core::ContentType::CHARACTERSHEET, QObject::tr("Charecter Sheet"));
    names.insert(Core::ContentType::MINDMAP, QObject::tr("Mindmap"));
    names.insert(Core::ContentType::SHAREDNOTE, QObject::tr("Shared Notes"));
#ifdef WITH_PDF
    names.insert(Core::ContentType::PDF, QObject::tr("Pdf"));
#endif
    names.insert(Core::ContentType::WEBVIEW, QObject::tr("Webview"));

    return names.value(type);
}

Core::ContentType mediaTypeToContentType(Core::MediaType type)
{
    auto res= Core::ContentType::UNKNOWN;

    switch(type)
    {
    case Core::MediaType::Unknown:
    case Core::MediaType::TokenFile:
    case Core::MediaType::PlayListFile:
    case Core::MediaType::AudioFile:
        res= Core::ContentType::UNKNOWN;
        break;
    case Core::MediaType::ImageFile:
        res= Core::ContentType::PICTURE;
        break;
    case Core::MediaType::MapFile:
        res= Core::ContentType::VECTORIALMAP;
        break;
    case Core::MediaType::TextFile:
        res= Core::ContentType::NOTES;
        break;
    case Core::MediaType::CharacterSheetFile:
        res= Core::ContentType::CHARACTERSHEET;
        break;
    case Core::MediaType::PdfFile:
        res= Core::ContentType::PDF;
        break;
    case Core::MediaType::WebpageFile:
        res= Core::ContentType::WEBVIEW;
        break;
    case Core::MediaType::MindmapFile:
        res= Core::ContentType::MINDMAP;
        break;
    }

    return res;
}

QString typeToIconPath(Core::ContentType type)
{
    auto hash= QHash<Core::ContentType, QString>({
        {Core::ContentType::VECTORIALMAP, "vmap"},
        {Core::ContentType::PICTURE, "photo"},
        //{Core::ContentType::ONLINEPICTURE, "photo"},
        {Core::ContentType::NOTES, "notes"},
        {Core::ContentType::CHARACTERSHEET, "treeview"},
        {Core::ContentType::SHAREDNOTE, "sharedEditor"},
        {Core::ContentType::WEBVIEW, "webPage"},
#ifdef WITH_PDF
        {Core::ContentType::PDF, "pdfLogo"},
#endif
    });
    return hash.value(type);
}

Core::ContentType extensionToContentType(const QString& filename)
{
    QSet<Core::ContentType> types{Core::ContentType::VECTORIALMAP, Core::ContentType::PICTURE,
                                  Core::ContentType::NOTES,        Core::ContentType::MINDMAP,
                                  Core::ContentType::SHAREDNOTE,   Core::ContentType::PDF,
                                  Core::ContentType::WEBVIEW,      Core::ContentType::CHARACTERSHEET};
    for(auto type : types)
    {
        auto list= extentionPerType(type, false);
        for(const auto& ext : list)
        {
            if(filename.endsWith(ext))
                return type;
        }
    }
    return Core::ContentType::UNKNOWN;
}

} // namespace utils
} // namespace helper
