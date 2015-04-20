/**********************************************************************************
 * Copyright © 2015 Pavel A. Puchkov                                              *
 *                                                                                *
 * This file is part of IqOrm.                                                    *
 *                                                                                *
 * IqOrm is free software: you can redistribute it and/or modify                  *
 * it under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or              *
 * (at your option) any later version.                                            *
 *                                                                                *
 * IqOrm is distributed in the hope that it will be useful,                       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 * GNU Lesser General Public License for more details.                            *
 *                                                                                *
 * You should have received a copy of the GNU Lesser General Public License       *
 * along with IqOrm.  If not, see <http://www.gnu.org/licenses/>.                 *
 **********************************************************************************/

#include "iqormobjectprivateaccessor.h"
#include "iqormobject.h"
#include "iqormobjectrawdata.h"

namespace IqOrmPrivate {

void IqOrmObjectPrivateAccessor::setObjectId(IqOrmObject *object, const int id)
{
    Q_CHECK_PTR(object);
    object->setObjectId(id);
}

void IqOrmObjectPrivateAccessor::setVaules(IqOrmObject *object, const IqOrmObjectRawData &rawData)
{
    Q_CHECK_PTR(object);
    object->setValues(rawData);
}

void IqOrmObjectPrivateAccessor::updateObjectSourceProperites(IqOrmObject *object)
{
    Q_CHECK_PTR(object);
    object->updateSourcePropertyValues();
}

void IqOrmObjectPrivateAccessor::resetObject(IqOrmObject *object)
{
    Q_CHECK_PTR(object);
    object->reset();
}

QVariant IqOrmObjectPrivateAccessor::sourcePropertyValue(const IqOrmObject *object,
                                                         const IqOrmPropertyDescription *propertyDescription)
{
    Q_CHECK_PTR(object);
    return object->sourcePropertyValue(propertyDescription);
}

bool IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(const IqOrmObject *object)
{
    Q_CHECK_PTR(object);
    return object->isLoadedFromDataSource();
}

void IqOrmObjectPrivateAccessor::setObjectIsLoadedFromDataSource(IqOrmObject *object, bool isLoadedFromDataSource)
{
    Q_CHECK_PTR(object);
    object->setIsLoadedFromDataSource(isLoadedFromDataSource);
}

bool IqOrmObjectPrivateAccessor::isObjectSavedToDataSource(const IqOrmObject *object)
{
    Q_CHECK_PTR(object);
    return object->isSavedToDataSource();
}

}
