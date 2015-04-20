#include "iqormmodelprivateaccessor.h"

namespace IqOrmPrivate {

void IqOrmModelPrivateAccessor::setObjectValues(IqOrmBaseModel *model, const QList<IqOrmObjectRawData> &objectValues)
{
    Q_CHECK_PTR(model);
    model->setObjectsValues(objectValues);
}

}
