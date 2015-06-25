#ifndef IQORMMETAMODELPRIVATEACCESSOR_H
#define IQORMMETAMODELPRIVATEACCESSOR_H

#include "iqormmetamodel.h"
#include "iqorm_global.h"

namespace IqOrmPrivate {

class IQORMSHARED_EXPORT IqOrmMetaModelPrivateAccessor
{
public:
    static const QMetaObject *targetStaticMetaObject(const IqOrmMetaModel *metaModel);
    static void setTargetStaticMetaObject(IqOrmMetaModel *metaModel,
                                          const QMetaObject *targetStaticMetaObject);

    static QSet<const IqOrmPropertyDescription *> propertyDescriptions(const IqOrmMetaModel *metaModel);

    static const IqOrmPropertyDescription *propertyDescription(const IqOrmMetaModel *metaModel,
                                                               const QString &propertyName);

    static bool isValid(const IqOrmMetaModel *metaModel);
};

}

#endif // IQORMMETAMODELPRIVATEACCESSOR_H
