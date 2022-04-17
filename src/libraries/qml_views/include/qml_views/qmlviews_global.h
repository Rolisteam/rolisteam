#ifndef QMLVIEWS_GLOBAL_H
#define QMLVIEWS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QMLVIEWS_LIBRARY)
#define QMLVIEWS_EXPORT Q_DECL_EXPORT
#else
#define QMLVIEWS_EXPORT Q_DECL_IMPORT
#endif
#endif // QMLVIEWS_GLOBAL_H
