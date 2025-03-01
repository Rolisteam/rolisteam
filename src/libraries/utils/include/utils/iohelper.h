#ifndef UTIL_IOHELPER_H
#define UTIL_IOHELPER_H

#include "utils/utils_global.h"

#include <QImage>
#include <QPixmap>
#include <QString>
#include <QUrl>

namespace utils
{
namespace IOHelper
{
// file API
UTILS_EXPORT QByteArray loadFile(const QString& file);
UTILS_EXPORT bool writeFile(const QString& path, const QByteArray& arry, bool override= true);
UTILS_EXPORT bool moveFile(const QString& source, const QString& destination);
UTILS_EXPORT bool removeFile(const QString& soursce);
UTILS_EXPORT bool moveFilesToDirectory(const QString& files, const QString& dest);
UTILS_EXPORT QString copyFile(const QString& source, const QString& destination, bool overwrite= false);
UTILS_EXPORT bool makeDir(const QString& dir);
UTILS_EXPORT QString shortNameFromPath(const QString& path);
UTILS_EXPORT QString shortNameFromUrl(const QUrl& url);
UTILS_EXPORT QString absoluteToRelative(const QString& absolute, const QString& root);
UTILS_EXPORT QString readTextFile(const QString& file);
UTILS_EXPORT QPixmap readPixmapFromURL(const QUrl& url);
UTILS_EXPORT bool savePixmapInto(const QPixmap& map, const QUrl& destination);
UTILS_EXPORT QPixmap readPixmapFromFile(const QString& url);
UTILS_EXPORT QImage dataToImage(const QByteArray& data);
UTILS_EXPORT QByteArray imageToData(const QImage& pix);
UTILS_EXPORT QString fileNameFromPath(const QString& path);
} // namespace IOHelper
} // namespace utils
#endif // UTIL_IOHELPER_H
