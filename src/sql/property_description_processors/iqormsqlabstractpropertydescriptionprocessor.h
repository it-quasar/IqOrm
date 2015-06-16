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

#ifndef IQORMSQLABSTRACTPROPERTYDESCRIPTIONPROCESSOR_H
#define IQORMSQLABSTRACTPROPERTYDESCRIPTIONPROCESSOR_H

#include <QObject>
#include <QSqlQuery>

#include "iqormpropertydescription.h"
#include "iqormdatasourceoperationresult.h"
#include "iqormsqldatasource.h"
#include "iqormobject.h"
#include "iqormsqljoinoperation.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlAbstractPropertyDescriptionProcessor
{
public:
    explicit IqOrmSqlAbstractPropertyDescriptionProcessor();

    virtual ~IqOrmSqlAbstractPropertyDescriptionProcessor();

    virtual QVariant propertyValue() const = 0;

    virtual QString selectFieldName() const = 0;

    virtual QString insertFieldName() const = 0;

    virtual QString updateFieldName() const = 0;

    virtual QVariant insertValue() const = 0;

    virtual QVariant updateValue() const = 0;

    virtual QList<IqOrmSqlJoinOperation> selectJoinOperations() const = 0;

    virtual bool selectAllowed(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool insertAllowed(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool updateAllowed(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool removeAllowed(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool preSelect(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool preInsert(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool preUpdate(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool preRemove(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool postSelect(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool postInsert(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool postUpdate(IqOrmDataSourceOperationResult *result) const = 0;

    virtual bool postRemove(IqOrmDataSourceOperationResult *result) const = 0;

    virtual QVariant convertSqlValue(const QVariant &sqlValue,
                                     bool *ok = Q_NULLPTR,
                                     QString *error = Q_NULLPTR) const = 0;

public:
    IqOrmSqlDataSource *sqlDataSource() const;
    void setSqlDataSource(IqOrmSqlDataSource *sqlDataSource);

    const IqOrmPropertyDescription *propertyDescription() const;
    void setPropertyDescription(const IqOrmPropertyDescription *propertyDescription);

    const IqOrmObject *object() const;
    void setObject(const IqOrmObject *object);

    const IqOrmMetaModel *ormModel() const;
    void setOrmModel(const IqOrmMetaModel *ormModel);

protected:
    bool checkPropertySourceValueIsValid(IqOrmDataSourceOperationResult *opertionResult) const;

    bool checkPropetyValueNotNull(IqOrmDataSourceOperationResult *opertionResult) const;

    bool checkPropetyNotContainNullObjects(IqOrmDataSourceOperationResult *opertionResult) const;

    bool checkPropertyValueObjectIsWasSavedToDb(IqOrmDataSourceOperationResult *opertionResult) const;

    bool checkPropertyValueObjectsListIsWasSavedToDb(IqOrmDataSourceOperationResult *opertionResult) const;

    bool propertyWasChanged() const;

    QString escapedIdFieldName() const;

    QString escapedFieldName(const QString &fieldName) const;

    QString escapedTableName(const QString &tableName) const;

    QString escapedPropertyName(const QString &propertyName) const;

    QVariant excapedValue(const QVariant &value) const;

    QString escapedObjectTableName() const;

    QString generateFieldStringAggColumn(const QString &tableName,
                                         const QString &fieldName,
                                         const QString &joinCondition,
                                         const QString &asName,
                                         bool *needJoin) const;

    QSqlQuery execQuery(const QString &prepareString,
                        const QList<QVariant> bindValues,
                        bool forwardOnly,
                        bool *ok,
                        QString *errorText) const;

    QSqlQuery prepareQuery(const QString &prepareString,
                           bool *ok = Q_NULLPTR,
                           QString *errorText = Q_NULLPTR) const;

    bool execPreparedQuery(QSqlQuery &query,
                           const QList<QVariant> bindValues = QList<QVariant>(),
                           QString *errorText = Q_NULLPTR) const;


private:
    IqOrmSqlDataSource *m_sqlDataSource;
    const IqOrmPropertyDescription *m_propertyDescription;
    const IqOrmObject *m_object;
    const IqOrmMetaModel *m_ormModel;
};

#endif // IQORMSQLABSTRACTPROPERTYDESCRIPTIONPROCESSOR_H
