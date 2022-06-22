#ifndef COMMON_QML_GLOBAL_H
#define COMMON_QML_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(COMMON_QML_LIBRARY)
#define COMMON_QML_EXPORT Q_DECL_EXPORT
#else
#define COMMON_QML_EXPORT Q_DECL_IMPORT
#endif
#endif // COMMON_QML_GLOBAL_H
