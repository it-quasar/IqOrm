#include "iqormmetamodelprivateaccessor.h"

namespace IqOrmPrivate {

const QMetaObject *IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(const IqOrmMetaModel *metaModel)
{
    Q_CHECK_PTR(metaModel);
    return metaModel->targetStaticMetaObject();
}

void IqOrmMetaModelPrivateAccessor::setTargetStaticMetaObject(IqOrmMetaModel *metaModel, const QMetaObject *targetStaticMetaObject)
{
    Q_CHECK_PTR(metaModel);
    metaModel->setTargetStaticMetaObject(targetStaticMetaObject);
}

QSet<const IqOrmPropertyDescription *> IqOrmMetaModelPrivateAccessor::propertyDescriptions(const IqOrmMetaModel *metaModel)
{
    Q_CHECK_PTR(metaModel);
    return metaModel->propertyDescriptions();
}

const IqOrmPropertyDescription *IqOrmMetaModelPrivateAccessor::propertyDescription(const IqOrmMetaModel *metaModel, const QString &propertyName)
{
    Q_CHECK_PTR(metaModel);
    return metaModel->propertyDescription(propertyName);
}

bool IqOrmMetaModelPrivateAccessor::isValid(const IqOrmMetaModel *metaModel)
{
    Q_CHECK_PTR(metaModel);
    return metaModel->isValid();
}

}
