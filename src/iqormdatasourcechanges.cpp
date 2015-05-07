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

#include "iqormdatasourcechanges.h"
#include <QDebug>

IqOrmDataSourceChanges::IqOrmDataSourceChangesData::IqOrmDataSourceChangesData() :
    dataSource(Q_NULLPTR),
    objectId(-1),
    objectOrmModel(Q_NULLPTR),
    operation(IqOrmAbstractDataSource::NotSetOperation)
{
}

IqOrmDataSourceChanges::IqOrmDataSourceChangesData::~IqOrmDataSourceChangesData()
{
    foreach (IqOrmPropertyChanges *changes, propertiesChanges.values()) {
        delete changes;
    }
}

IqOrmDataSourceChanges::IqOrmDataSourceChanges() :
    d (new IqOrmDataSourceChangesData())
{
}

IqOrmAbstractDataSource *IqOrmDataSourceChanges::dataSource() const
{
    return d->dataSource;
}

void IqOrmDataSourceChanges::setDataSource(IqOrmAbstractDataSource *dataSource)
{
    if (d->dataSource != dataSource)
        d->dataSource = dataSource;
}

int IqOrmDataSourceChanges::objectId() const
{
    return d->objectId;
}

void IqOrmDataSourceChanges::setObjectId(int objectId)
{
    if (d->objectId != objectId)
        d->objectId = objectId;
}

const IqOrmMetaModel *IqOrmDataSourceChanges::objectOrmModel() const
{
    return d->objectOrmModel;
}

void IqOrmDataSourceChanges::setObjectOrmModel(const IqOrmMetaModel *objectOrmModel)
{
    if (d->objectOrmModel != objectOrmModel)
        d->objectOrmModel = objectOrmModel;
}

IqOrmAbstractDataSource::Operation IqOrmDataSourceChanges::operation() const
{
    return d->operation;
}

void IqOrmDataSourceChanges::setOperation(const IqOrmAbstractDataSource::Operation &operation)
{
    if (d->operation != operation)
        d->operation = operation;
}

IqOrmDirectPropertyChanges *IqOrmDataSourceChanges::propertyChanges(const IqOrmDirectPropertyDescription *propetyDescription)
{
    return propertyChanges<IqOrmDirectPropertyChanges>(propetyDescription);
}

IqOrmOneToOnePropertyChanges *IqOrmDataSourceChanges::propertyChanges(const IqOrmBaseOneToOnePropertyDescription *propertyDescription)
{
    return propertyChanges<IqOrmOneToOnePropertyChanges>(propertyDescription);
}

IqOrmInversedOneToOnePropertyChanges *IqOrmDataSourceChanges::propertyChanges(const IqOrmBaseInversedOneToOnePropertyDescription *propertyDescription)
{
    return propertyChanges<IqOrmInversedOneToOnePropertyChanges>(propertyDescription);
}

IqOrmOneToManyPropertyChanges *IqOrmDataSourceChanges::propertyChanges(const IqOrmBaseOneToManyPropertyDescription *propertyDescription)
{
    return propertyChanges<IqOrmOneToManyPropertyChanges>(propertyDescription);
}

IqOrmManyToOnePropertyChanges *IqOrmDataSourceChanges::propertyChanges(const IqOrmBaseManyToOnePropertyDescription *propertyDescription)
{
    return propertyChanges<IqOrmManyToOnePropertyChanges>(propertyDescription);
}

IqOrmManyToManyPropertyChanges *IqOrmDataSourceChanges::propertyChanges(const IqOrmBaseManyToManyPropertyDescription *propertyDescription)
{
    return propertyChanges<IqOrmManyToManyPropertyChanges>(propertyDescription);
}

IqOrmInversedManyToManyPropertyChanges *IqOrmDataSourceChanges::propertyChanges(const IqOrmBaseInversedManyToManyPropertyDescription *propertyDescription)
{
    return propertyChanges<IqOrmInversedManyToManyPropertyChanges>(propertyDescription);
}

QSet<IqOrmDirectPropertyChanges *> IqOrmDataSourceChanges::directPropertiesChanges() const
{
    return propertiesChanges<IqOrmDirectPropertyChanges>();
}

QSet<IqOrmOneToOnePropertyChanges *> IqOrmDataSourceChanges::oneToOnePropertiesChanges() const
{
    return propertiesChanges<IqOrmOneToOnePropertyChanges>();
}

QSet<IqOrmInversedOneToOnePropertyChanges *> IqOrmDataSourceChanges::inversedOneToOnePropertiesChanges() const
{
    return propertiesChanges<IqOrmInversedOneToOnePropertyChanges>();
}

QSet<IqOrmOneToManyPropertyChanges *> IqOrmDataSourceChanges::oneToManyPropertiesChanges() const
{
    return propertiesChanges<IqOrmOneToManyPropertyChanges>();
}

QSet<IqOrmManyToOnePropertyChanges *> IqOrmDataSourceChanges::manyToOnePropertiesChanges() const
{
    return propertiesChanges<IqOrmManyToOnePropertyChanges>();
}

QSet<IqOrmManyToManyPropertyChanges *> IqOrmDataSourceChanges::manyToManyPropertiesChanges() const
{
    return propertiesChanges<IqOrmManyToManyPropertyChanges>();
}

QSet<IqOrmInversedManyToManyPropertyChanges *> IqOrmDataSourceChanges::inversedManyToManyPropertiesChanges() const
{
    return propertiesChanges<IqOrmInversedManyToManyPropertyChanges>();
}
