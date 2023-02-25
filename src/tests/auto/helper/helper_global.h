#ifndef HELPER_GLOBAL_H
#define HELPER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HELPER_LIBRARY)
#define HELPER_EXPORT Q_DECL_EXPORT
#else
#define HELPER_EXPORT Q_DECL_IMPORT
#endif
#endif // HELPER_GLOBAL_H
