#ifndef IM_GLOBAL_H
#define IM_GLOBAL_H
#include <QtCore/qglobal.h>

#if defined(IM_LIBRARY)
#define IM_EXPORT Q_DECL_EXPORT
#else
#define IM_EXPORT Q_DECL_IMPORT
#endif
#endif // IM_GLOBAL_H
