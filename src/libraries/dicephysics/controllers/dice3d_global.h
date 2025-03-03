#ifndef DICE3D_GLOBAL_H
#define DICE3D_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(DICE3D_LIBRARY)
#define DICE3D_EXPORTS Q_DECL_EXPORT
#else
#define DICE3D_EXPORTS Q_DECL_IMPORT
#endif

#endif // DICE3D_GLOBAL_H
