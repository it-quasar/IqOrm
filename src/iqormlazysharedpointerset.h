#ifndef IQORMLAZYSHAREDPOINTERSET_H
#define IQORMLAZYSHAREDPOINTERSET_H

#include <QSet>
#include "iqormlazysharedpointer.h"
#include "iqorm_global.h"

template <class T>
class IQORMSHARED_EXPORT IqOrmLazySharedPointerSet : public QSet<IqOrmLazySharedPointer<T> >
{
};

#endif // IQORMLAZYSHAREDPOINTERSET_H
