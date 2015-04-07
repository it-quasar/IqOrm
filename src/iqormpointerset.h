#ifndef IQORMPOINTERSET_H
#define IQORMPOINTERSET_H

#include <QSet>
#include <QPointer>
#include "iqorm_global.h"

template <class T>
class IQORMSHARED_EXPORT IqOrmPointerSet : public QSet<QPointer<T> >
{
};

#endif // IQORMPOINTERSET_H
