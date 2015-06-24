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

#include "iqormabstractobjectdatasource.h"
#include "iqormabstractdatasource.h"
#include "iqormpropertydescription.h"
#include "iqormmetamodel.h"
#include "iqormerror.h"
#include "iqormobject.h"
#include "iqormobjectprivateaccessor.h"
#include "iqormdatasourceoperationresult.h"
#include <QDebug>

IqOrmAbstractObjectDataSource::IqOrmAbstractObjectDataSource(IqOrmAbstractDataSource *parent) :
    QObject(parent)
{
}

IqOrmAbstractObjectDataSource::~IqOrmAbstractObjectDataSource()
{
}

//bool IqOrmAbstractObjectDataSource::loadObjectAllManyToManyReferences(IqOrmObject *object) const
//{
//    if (!object)
//    {
//        qWarning() << tr("IqOrmAbstractObjectDataSource::loadObjectAllManyToManyReferences() called without object.");
//        return false;
//    }

//    resetObjectReference(object);

//    IqOrmModel *ormModel = object->ormModel();
//    if (!ormModel)
//    {
//        object->lastError()->addError(tr("Not found valid object ORM model."));
//        return false;
//    }

//    foreach (IqOrmPropertyDescription *propDescription, ormModel->propertyDescriptions())
//    {
//        if (!propDescription || propDescription->propertyType() != IqOrmPropertyDescription::ManyToManyReference)
//            continue;

//        if(!loadObjectManyToManyReference(object, propDescription->propertyName()))
//        {
//            resetObjectReference(object);
//            return false;
//        }
//    }

//    return true;
//}
