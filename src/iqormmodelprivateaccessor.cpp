#include "iqormmodelprivateaccessor.h"

namespace IqOrmPrivate {
void IqOrmModelPrivateAccessor::append(IqOrmBaseModel *model, IqOrmObject *object)
{
    Q_CHECK_PTR(model);
    model->append(object);
}

void IqOrmModelPrivateAccessor::insert(IqOrmBaseModel *model, int row, IqOrmObject *object)
{
    Q_CHECK_PTR(model);
    model->insert(row, object);
}

void IqOrmModelPrivateAccessor::remove(IqOrmBaseModel *model, IqOrmObject *object)
{
    Q_CHECK_PTR(model);
    model->remove(object);
}

void IqOrmModelPrivateAccessor::clear(IqOrmBaseModel *model)
{
    Q_CHECK_PTR(model);
    model->clear();
}

IqOrmObject *IqOrmModelPrivateAccessor::createChildObject(IqOrmBaseModel *model)
{
    Q_CHECK_PTR(model);
    return model->createChildObject();
}
}
