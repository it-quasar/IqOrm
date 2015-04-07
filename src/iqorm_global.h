#ifndef IQORM_GLOBAL_H
#define IQORM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IQORM_LIBRARY)
#  define IQORMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IQORMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IQORM_GLOBAL_H
