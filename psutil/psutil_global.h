#ifndef PSUTIL_GLOBAL_H
#define PSUTIL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PSUTIL_LIBRARY)
#  define PSUTILSHARED_EXPORT Q_DECL_EXPORT
#else
#  define PSUTILSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // PSUTIL_GLOBAL_H
