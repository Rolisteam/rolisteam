#include "utils/iohelper.h"
#include "utils/networkdownloader.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#include <QSignalSpy>


namespace utils
{
namespace IOHelper
{
QByteArray loadFile(const QString& filepath)
{
    QByteArray data;
    if(filepath.isEmpty())
        return data;
    QFile file(filepath);
    if(file.open(QIODevice::ReadOnly))
    {
        data= file.readAll();
    }
    return data;
}

bool makeSurePathExist(const QDir& dir)
{
    if(!dir.exists())
        dir.mkpath(dir.path());

    return dir.exists();
}

bool makeDir(const QString& dir)
{
    return makeSurePathExist(QDir(dir));
}

QString copyFile(const QString& source, const QString& destination)
{
    QFile src(source);
    QFileInfo srcInfo(source);
    QFileInfo dest(destination);
    if(dest.isDir())
        dest.setFile(QString("%1/%2").arg(destination, srcInfo.fileName()));

    if(makeSurePathExist(dest.dir()))
    {
        if(src.copy(dest.absoluteFilePath()))
            return dest.absoluteFilePath();
    }
    return {};
}

bool removeFile(const QString& source)
{
    return QFile::remove(source);
}

bool writeFile(const QString& path, const QByteArray& arry, bool override)
{
    if(arry.isEmpty())
        return false;

    QFileInfo pathInfo(path);
    makeDir(pathInfo.absolutePath());

    QSaveFile file(path);
    if(file.open(override ? QIODevice::WriteOnly : QIODevice::Append))
    {
        file.write(arry);
        file.commit();
        return true;
    }
    return false;
}

bool moveFile(const QString& source, const QString& destination)
{
    QFile src(source);
    QFileInfo srcInfo(source);
    QFileInfo dest(destination);

    if(dest.isDir())
        dest.setFile(QString("%1/%2").arg(destination, srcInfo.fileName()));

    bool res= false;
    if(makeSurePathExist(dest.dir()))
    {
        res= src.rename(dest.absoluteFilePath());
    }
    return res;
}

QString readTextFile(const QString& path)
{
    if(path.isEmpty())
        return {};
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return {};

    QTextStream out(&file);
    return out.readAll();
}

QPixmap readPixmapFromFile(const QString& uri)
{
    return QPixmap(uri);
}

QPixmap readPixmapFromWeb(const QUrl& url)
{
    NetworkDownloader dl(url);
    QSignalSpy spy(&dl, &NetworkDownloader::finished);

    spy.wait(5000);
    if(spy.count() == 0)
        return {};
    else
    {
        auto first = spy.takeFirst();
        auto hasImage = first.at(1).toBool();
        auto imageData = first.at(0).toByteArray();
        if(hasImage)
            return QPixmap::fromImage(dataToImage(imageData));
    }
    return {};
}

QPixmap readPixmapFromURL(const QUrl& url)
{
    QPixmap map;
    if(url.isLocalFile())
    {
        map= readPixmapFromFile(url.toLocalFile());
    }
    else if(url.scheme() == QStringLiteral("qrc"))
    {
        map= readPixmapFromFile(url.path());
    }
    else if(url.scheme().startsWith(QStringLiteral("http")))
    {
        map = readPixmapFromWeb(url);
    }
    return map;
}
QString shortNameFromPath(const QString& path)
{
    QFileInfo info(path);
    return info.baseName();
}
QString absoluteToRelative(const QString& absolute, const QString& root)
{
    QDir dir(root);
    QFileInfo path(absolute);
    if(!path.isAbsolute())
        return absolute;

    return dir.relativeFilePath(absolute);
}

QImage dataToImage(const QByteArray& data)
{
    QImage img;
    img.loadFromData(data);
    return img;
}
} // namespace IOHelper
} // namespace utils
