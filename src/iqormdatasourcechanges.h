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

#ifndef IQORMDATASOURCECHANGES_H
#define IQORMDATASOURCECHANGES_H

#include <QSharedData>
#include "iqormmetamodel.h"
#include "iqormdirectpropertydescription.h"

#include "iqormdirectpropertychanges.h"
#include "iqormonetoonepropertychanges.h"
#include "iqorminversedonetoonepropertychanges.h"
#include "iqormmanytoonepropertychanges.h"
#include "iqormonetomanypropertychanges.h"
#include "iqormmanytomanypropertychanges.h"
#include "iqorminversedmanytomanypropertychanges.h"
#include "iqorm_global.h"

class IqOrmAbstractDataSource;
class IQORMSHARED_EXPORT IqOrmDataSourceChanges
{
public:
    enum Operation
    {
        Persist,
        Update,
        Remove
    };

    IqOrmDataSourceChanges();

public:
    IqOrmAbstractDataSource *dataSource() const;
    void setDataSource(IqOrmAbstractDataSource *dataSource);

    int objectId() const;
    void setObjectId(int objectId);

    const IqOrmMetaModel *objectOrmModel() const;
    void setObjectOrmModel(const IqOrmMetaModel *objectOrmModel);

    Operation operation() const;
    void setOperation(const Operation &operation);

    IqOrmDirectPropertyChanges *propertyChanges(const IqOrmDirectPropertyDescription *propetyDescription);
    IqOrmOneToOnePropertyChanges *propertyChanges(const IqOrmBaseOneToOnePropertyDescription *propertyDescription);
    IqOrmInversedOneToOnePropertyChanges *propertyChanges(const IqOrmBaseInversedOneToOnePropertyDescription *propertyDescription);
    IqOrmOneToManyPropertyChanges *propertyChanges(const IqOrmBaseOneToManyPropertyDescription *propertyDescription);
    IqOrmManyToOnePropertyChanges *propertyChanges(const IqOrmBaseManyToOnePropertyDescription *propertyDescription);
    IqOrmManyToManyPropertyChanges *propertyChanges(const IqOrmBaseManyToManyPropertyDescription *propertyDescription);
    IqOrmInversedManyToManyPropertyChanges *propertyChanges(const IqOrmBaseInversedManyToManyPropertyDescription *propertyDescription);
    template <class T>
    T * propertyChanges(const IqOrmPropertyDescription *propertyDescription);

    QSet<IqOrmDirectPropertyChanges *> directPropertiesChanges() const;
    QSet<IqOrmOneToOnePropertyChanges *> oneToOnePropertiesChanges() const;
    QSet<IqOrmInversedOneToOnePropertyChanges *> inversedOneToOnePropertiesChanges() const;
    QSet<IqOrmOneToManyPropertyChanges *> oneToManyPropertiesChanges() const;
    QSet<IqOrmManyToOnePropertyChanges *> manyToOnePropertiesChanges() const;
    QSet<IqOrmManyToManyPropertyChanges *> manyToManyPropertiesChanges() const;
    QSet<IqOrmInversedManyToManyPropertyChanges *> inversedManyToManyPropertiesChanges() const;

private:
    template <class T>
    QSet<T *> propertiesChanges() const;

private:
    class IqOrmDataSourceChangesData : public QSharedData
    {
    public:
        IqOrmDataSourceChangesData();
        ~IqOrmDataSourceChangesData();

        IqOrmAbstractDataSource *dataSource;
        int objectId;
        const IqOrmMetaModel *objectOrmModel;
        Operation operation;
        QHash<const IqOrmPropertyDescription *, IqOrmPropertyChanges *> propertiesChanges;
    };

    QSharedDataPointer<IqOrmDataSourceChangesData> d;
};




template <class T>
QSet<T *> IqOrmDataSourceChanges::propertiesChanges() const
{
    QSet<T *> result;
    foreach (IqOrmPropertyChanges *changes, d->propertiesChanges.values()) {
        T *castChanges = dynamic_cast<T *>(changes);
        if (castChanges)
            result << castChanges;
    }
    return result;
}

template <class T>
T * IqOrmDataSourceChanges::propertyChanges(const IqOrmPropertyDescription *propertyDescription)
{
    if (d->propertiesChanges.contains(propertyDescription))
        return dynamic_cast<T *>(d->propertiesChanges[propertyDescription]);

    T* newChanges = new T();
    if (true)
        Q_CHECK_PTR(dynamic_cast<IqOrmPropertyChanges *>(newChanges));

    newChanges->setPropertyDescription(propertyDescription);
    d->propertiesChanges[propertyDescription] = newChanges;

    return newChanges;
}

#endif // IQORMDATASOURCECHANGES_H
