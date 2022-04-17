#ifndef UTIL_IOHELPER_H
#define UTIL_IOHELPER_H

#include <util_global.h>

#include <QImage>
#include <QPixmap>
#include <QString>
#include <QUrl>

namespace utils
{
namespace IOHelper
{
// file API
UTIL_EXPORT QByteArray loadFile(const QString& file);
UTIL_EXPORT void writeFile(const QString& path, const QByteArray& arry, bool override= true);
UTIL_EXPORT bool moveFile(const QString& source, const QString& destination);
UTIL_EXPORT bool removeFile(const QString& soursce);
UTIL_EXPORT bool moveFilesToDirectory(const QString& files, const QString& dest);
UTIL_EXPORT QString copyFile(const QString& source, const QString& destination);
UTIL_EXPORT bool makeDir(const QString& dir);
UTIL_EXPORT QString shortNameFromPath(const QString& path);
UTIL_EXPORT QString shortNameFromUrl(const QUrl& url);
UTIL_EXPORT QString absoluteToRelative(const QString& absolute, const QString& root);
UTIL_EXPORT QString readTextFile(const QString& file);
UTIL_EXPORT QPixmap readPixmapFromURL(const QUrl& url);
UTIL_EXPORT QPixmap readPixmapFromFile(const QString& url);
UTIL_EXPORT QImage dataToImage(const QByteArray& data);
} // namespace IOHelper
} // namespace utils
#endif // UTIL_IOHELPER_H
