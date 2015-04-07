#ifndef IQORMLAZYPOINTERSET_H
#define IQORMLAZYPOINTERSET_H

#include <QSet>
#include "iqormlazypointer.h"
#include "iqorm_global.h"

template <class T>
class IQORMSHARED_EXPORT IqOrmLazyPointerSet : public QSet<IqOrmLazyPointer<T> >
{
};

#endif // IQORMLAZYPOINTERSET_H
