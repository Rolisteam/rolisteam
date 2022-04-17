#ifndef RWIDGETS_GLOBAL_H
#define RWIDGETS_GLOBAL_H
#include <QtCore/qglobal.h>

#if defined(RWIDGET_LIBRARY)
#define RWIDGET_EXPORT Q_DECL_EXPORT
#else
#define RWIDGET_EXPORT Q_DECL_IMPORT
#endif
#endif // RWIDGETS_GLOBAL_H
