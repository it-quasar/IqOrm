#ifndef IQORMSHAREDLAZYPOINTERSET_H
#define IQORMSHAREDLAZYPOINTERSET_H

#include <QSet>
#include "iqormsharedlazypointer.h"
#include "iqorm_global.h"

template <class T>
class IQORMSHARED_EXPORT IqOrmSharedLazyPointerSet : public QSet<IqOrmSharedLazyPointer<T> >
{
};

#endif // IQORMLAZYSHAREDPOINTERSET_H
