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

#ifndef IQORMSQLOBJECTDATASOURCE_H
#define IQORMSQLOBJECTDATASOURCE_H

#include "iqormabstractobjectdatasource.h"

#include "iqorm_global.h"
#include <QSqlRecord>
#include <QSqlQuery>
#include <QVariant>

class IqOrmSqlDataSource;
class IqOrmPropertyDescription;
class IqOrmMetaModel;
class IqOrmObjectRawData;
class IqOrmSqlPropertyDescriptionsProcessor;
class IqOrmOneObjectDescribingPropertyDescription;
class IqOrmManyObjectDescribingPropertyDescription;

class IQORMSHARED_EXPORT IqOrmSqlObjectDataSource : public IqOrmAbstractObjectDataSource
{
    Q_OBJECT
public:
    explicit IqOrmSqlObjectDataSource(IqOrmSqlDataSource *sqlDataSource = Q_NULLPTR);

    virtual IqOrmDataSourceOperationResult loadObjectById(IqOrmObject* object, qint64 id) const Q_DECL_OVERRIDE;

    virtual IqOrmDataSourceOperationResult updateObject(IqOrmObject* object, const QSet<const IqOrmPropertyDescription *> &properties) Q_DECL_OVERRIDE;

    virtual IqOrmDataSourceOperationResult insertObject(IqOrmObject *object) Q_DECL_OVERRIDE;

    virtual IqOrmDataSourceOperationResult removeObject(IqOrmObject* object) Q_DECL_OVERRIDE;

private:
    friend class IqOrmSqlModelDataSource;

    static bool loadObjectFromSqlQuery(IqOrmObject* object,
                                       const QSqlQuery &query,
                                       QString *error = Q_NULLPTR);

    static IqOrmObjectRawData createRawDataForObjectFromSqlQuery(const IqOrmMetaModel *objectOrmMetaModel,
                                                                 const QSqlQuery &query,
                                                                 bool *ok = Q_NULLPTR,
                                                                 QString *error = Q_NULLPTR);

    QString generateSelectQuery(const IqOrmMetaModel *ormModel, bool distinct = false) const;

    QString generateSelectCountQuery(const IqOrmMetaModel *ormModel) const;

private:
    IqOrmSqlDataSource* m_sqlDataSource;
    IqOrmSqlPropertyDescriptionsProcessor *m_propertyDescriptionsProcessor;
};

#endif // IQORMSQLOBJECTDATASOURCE_H
