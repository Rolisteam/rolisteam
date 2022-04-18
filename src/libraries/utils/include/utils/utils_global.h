#ifndef UTIL_GLOBAL_H
#define UTIL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(UTILS_LIBRARY)
#define UTILS_EXPORT Q_DECL_EXPORT
#else
#define UTILS_EXPORT Q_DECL_IMPORT
#endif
#endif // UTIL_GLOBAL_H
