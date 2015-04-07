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

#ifndef IQORMABSTRACTOBJECTDATASOURCE_H
#define IQORMABSTRACTOBJECTDATASOURCE_H

#include <QObject>
#include "iqormdatasourceoperationresult.h"
#include "iqorm_global.h"


class IqOrmAbstractDataSource;
class IqOrmObject;
class IqOrmPropertyDescription;

class IQORMSHARED_EXPORT IqOrmAbstractObjectDataSource : public QObject
{
    Q_OBJECT
public:
    explicit IqOrmAbstractObjectDataSource(IqOrmAbstractDataSource *parent = Q_NULLPTR);

    virtual ~IqOrmAbstractObjectDataSource();

    virtual IqOrmDataSourceOperationResult loadObjectById(IqOrmObject* object, qint64 id) const = 0;

//    virtual bool loadMappedProperty(IqOrmObject *object, const QString &property) const = 0;

//    virtual bool loadObjectManyToManyReference(IqOrmObject *object, const QString &referenceProperty) const = 0;

    virtual IqOrmDataSourceOperationResult updateObject(IqOrmObject* object, const QList<const IqOrmPropertyDescription *> &properties) = 0;

    virtual IqOrmDataSourceOperationResult insertObject(IqOrmObject *object) = 0;

    virtual IqOrmDataSourceOperationResult removeObject(IqOrmObject* object) = 0;

//    bool loadObjectAllManyToManyReferences(IqOrmObject *object) const;

protected:
    static void resetObject(IqOrmObject* object);

    static void resetObjectReference(IqOrmObject *object) Q_DECL_DEPRECATED;
};

#endif // IQORMABSTRACTOBJECTDATASOURCE_H
