#ifndef MEDIAHELPER_H
#define MEDIAHELPER_H

#include <QString>
#include <QVariant>
#include <core_global.h>
#include <map>

namespace MediaHelper
{
CORE_EXPORT std::map<QString, QVariant> prepareWebView(const QString& urlText);
};

#endif // MEDIAHELPER_H
