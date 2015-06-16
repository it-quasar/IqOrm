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

#ifndef IQORMSQLPROPERTYDESCRIPTIONSPROCESSOR_H
#define IQORMSQLPROPERTYDESCRIPTIONSPROCESSOR_H

#include <QObject>
#include "iqorm_global.h"

class IqOrmPropertyDescription;
class IqOrmSqlDataSource;
class IqOrmSqlAbstractPropertyDescriptionProcessor;
class IqOrmSqlJoinOperation;
class IqOrmDataSourceOperationResult;
class IqOrmObject;
class IqOrmMetaModel;

class IQORMSHARED_EXPORT IqOrmSqlPropertyDescriptionsProcessor : public QObject
{
    Q_OBJECT
public:
    explicit IqOrmSqlPropertyDescriptionsProcessor(QObject *parent = Q_NULLPTR);

    QStringList selectFieldNames(const IqOrmMetaModel *ormModel) const;

    QStringList insertFieldNames(const IqOrmMetaModel *ormModel) const;

    QStringList updateFieldNames(const IqOrmMetaModel *ormModel,
                                 const QSet<const IqOrmPropertyDescription *> &propertyDescriptions) const;

    QVariantList insertValues(const IqOrmObject *object) const;

    QVariantList updateValues(const IqOrmObject *object,
                              const QSet<const IqOrmPropertyDescription *> &propertyDescriptions) const;

    QList<IqOrmSqlJoinOperation> selectJoinOperations(const IqOrmMetaModel *ormModel) const;

    bool selectAllowed(const IqOrmObject *object,
                       IqOrmDataSourceOperationResult *result) const;

    bool insertAllowed(const IqOrmObject *object,
                       IqOrmDataSourceOperationResult *result) const ;

    bool updateAllowed(const IqOrmObject *object,
                       const QSet<const IqOrmPropertyDescription *> &propertyDescriptions,
                       IqOrmDataSourceOperationResult *result) const ;

    bool removeAllowed(const IqOrmObject *object,
                       IqOrmDataSourceOperationResult *result) const ;

    bool preSelect(const IqOrmObject *object,
                   IqOrmDataSourceOperationResult *result) const;

    bool preInsert(const IqOrmObject *object,
                   IqOrmDataSourceOperationResult *result) const;

    bool preUpdate(const IqOrmObject *object,
                   const QSet<const IqOrmPropertyDescription *> &propertyDescriptions,
                   IqOrmDataSourceOperationResult *result) const;

    bool preRemove(const IqOrmObject *object,
                   IqOrmDataSourceOperationResult *result) const;

    bool postSelect(const IqOrmObject *object,
                    IqOrmDataSourceOperationResult *result) const;

    bool postInsert(const IqOrmObject *object,
                    IqOrmDataSourceOperationResult *result) const;

    bool postUpdate(const IqOrmObject *object,
                    const QSet<const IqOrmPropertyDescription *> &propertyDescriptions,
                    IqOrmDataSourceOperationResult *result) const;

    bool postRemove(const IqOrmObject *object,
                    IqOrmDataSourceOperationResult *result) const;

    static QVariant convertSqlValueForProperty(const IqOrmPropertyDescription *propertyDescription,
                                               const QVariant &sqlValue,
                                               bool *ok = Q_NULLPTR,
                                               QString *error = Q_NULLPTR);


public:
    void setSqlDataSource(IqOrmSqlDataSource *sqlDataSource);

private:
    QSharedPointer<IqOrmSqlAbstractPropertyDescriptionProcessor> processor(const IqOrmPropertyDescription *propertyDescription,
                                                                           const IqOrmObject *object,
                                                                           const IqOrmMetaModel *ormModel = Q_NULLPTR) const;

    static QSharedPointer<IqOrmSqlAbstractPropertyDescriptionProcessor> processorForProperty(const IqOrmPropertyDescription *propertyDescription);

    IqOrmSqlDataSource *m_sqlDataSource;
};

#endif // IQORMSQLPROPERTYDESCRIPTIONSPROCESSOR_H
