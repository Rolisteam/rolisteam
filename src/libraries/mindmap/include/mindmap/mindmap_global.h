#ifndef MINDMAP_GLOBAL_H
#define MINDMAP_GLOBAL_H
#include <QtCore/qglobal.h>

#if defined(MINDMAP_LIBRARY)
#define MINDMAP_EXPORT Q_DECL_EXPORT
#else
#define MINDMAP_EXPORT Q_DECL_IMPORT
#endif
#endif // MINDMAP_GLOBAL_H
