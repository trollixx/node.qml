#ifndef NODEQML_GLOBAL_H
#define NODEQML_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(NODEQML_LIBRARY)
#  define NODEQMLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define NODEQMLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // NODEQML_GLOBAL_H
