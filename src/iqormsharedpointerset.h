#ifndef IQORMSHAREDPOINTERSET_H
#define IQORMSHAREDPOINTERSET_H

#include <QSet>
#include <QSharedPointer>
#include "iqorm_global.h"

template <class T>
class IQORMSHARED_EXPORT IqOrmSharedPointerSet : public QSet<QSharedPointer<T> >
{
};

#endif // IQORMSHAREDPOINTERSET_H
