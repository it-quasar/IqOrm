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

#include "iqormsqlabstractpropertydescriptionprocessor.h"
#include "iqormobjectprivateaccessor.h"
#include <QDebug>

IqOrmSqlAbstractPropertyDescriptionProcessor::IqOrmSqlAbstractPropertyDescriptionProcessor() :
    m_sqlDataSource(Q_NULLPTR),
    m_propertyDescription(Q_NULLPTR),
    m_object(Q_NULLPTR)
{
}

IqOrmSqlAbstractPropertyDescriptionProcessor::~IqOrmSqlAbstractPropertyDescriptionProcessor()
{
}

const IqOrmPropertyDescription *IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription() const
{
    return m_propertyDescription;
}

void IqOrmSqlAbstractPropertyDescriptionProcessor::setPropertyDescription(const IqOrmPropertyDescription *propertyDescription)
{
    m_propertyDescription = propertyDescription;
}

const IqOrmObject *IqOrmSqlAbstractPropertyDescriptionProcessor::object() const
{
    return m_object;
}

void IqOrmSqlAbstractPropertyDescriptionProcessor::setObject(const IqOrmObject *object)
{
    m_object = object;
    if (object)
        setOrmModel(object->ormMetaModel());
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::checkPropertySourceValueIsValid(IqOrmDataSourceOperationResult *opertionResult) const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(opertionResult);
    if (!propertyDescription()->value(object()).isValid()) {
        opertionResult->setError(QObject::tr("Property \"%0\" is not valid.")
                                 .arg(propertyDescription()->propertyName()));
        return false;
    }

    opertionResult->setError("");
    return true;
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::checkPropetyValueNotNull(IqOrmDataSourceOperationResult *opertionResult) const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(opertionResult);

    const IqOrmOneObjectDescribingPropertyDescription *oneObjectPropDescription =
            dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propertyDescription());
    Q_CHECK_PTR(oneObjectPropDescription);

    if (propertyDescription()->notNull() && oneObjectPropDescription->valueAsObjectId(object()).isNull()) {
        opertionResult->setError(QObject::tr("Property \"%0\" NOT NULL, but it set to Null.")
                                 .arg(propertyDescription()->propertyName()));
        return false;
    }

    opertionResult->setError("");
    return true;
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::checkPropetyNotContainNullObjects(IqOrmDataSourceOperationResult *opertionResult) const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(opertionResult);

    const IqOrmManyObjectDescribingPropertyDescription *manyObjectPropDescription =
            dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propertyDescription());
    Q_CHECK_PTR(manyObjectPropDescription);

    foreach (const QVariant &objectId, manyObjectPropDescription->valueAsObjectIds(object())) {
        if (objectId.isNull()) {
            opertionResult->setError(QObject::tr("Property \"%0\" contain one or more NULL object. For this property is not allowed.")
                                     .arg(propertyDescription()->propertyName()));
            return false;
        }
    }

    opertionResult->setError("");
    return true;
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::checkPropertyValueObjectIsWasSavedToDb(IqOrmDataSourceOperationResult *opertionResult) const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(opertionResult);

    const IqOrmOneObjectDescribingPropertyDescription *oneObjectPropDescription =
            dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propertyDescription());
    Q_CHECK_PTR(oneObjectPropDescription);

    if (!oneObjectPropDescription->valueAsObjectId(object()).isNull()
            && oneObjectPropDescription->valueAsObjectId(object()) == -1) {
        opertionResult->setError(QObject::tr("Object in property %0 not never stored in the database. Use "
                                    "save() for object in property %0 first.")
                                 .arg(propertyDescription()->propertyName()));
        return false;
    }

    opertionResult->setError("");
    return true;
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::checkPropertyValueObjectsListIsWasSavedToDb(IqOrmDataSourceOperationResult *opertionResult) const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(opertionResult);

    const IqOrmManyObjectDescribingPropertyDescription *manyObjectPropDescription =
            dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propertyDescription());
    Q_CHECK_PTR(manyObjectPropDescription);

    foreach (QVariant objectId, manyObjectPropDescription->valueAsObjectIds(object())) {
        if (objectId.toLongLong() == -1) {
            opertionResult->setError(QObject::tr("One or more objects in property %0 is not never stored in the database. Use "
                                        "save() for all objects in property %0 first.")
                                     .arg(propertyDescription()->propertyName()));
            return false;
        }
    }

    opertionResult->setError("");
    return true;
}

IqOrmSqlDataSource *IqOrmSqlAbstractPropertyDescriptionProcessor::sqlDataSource() const
{
    return m_sqlDataSource;
}

void IqOrmSqlAbstractPropertyDescriptionProcessor::setSqlDataSource(IqOrmSqlDataSource *sqlDataSource)
{
    m_sqlDataSource = sqlDataSource;
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::propertyWasChanged() const
{
    QVariant oldValue = IqOrmObjectPrivateAccessor::sourcePropertyValue(object(), propertyDescription());
    return oldValue != propertyValue();
}

QString IqOrmSqlAbstractPropertyDescriptionProcessor::escapedIdFieldName() const
{
    return sqlDataSource()->escapedIdFieldName();
}

QString IqOrmSqlAbstractPropertyDescriptionProcessor::escapedFieldName(const QString &fieldName) const
{
    return sqlDataSource()->escapedFieldName(fieldName);
}

QString IqOrmSqlAbstractPropertyDescriptionProcessor::escapedTableName(const QString &tableName) const
{
    return sqlDataSource()->escapedTableName(tableName);
}

QString IqOrmSqlAbstractPropertyDescriptionProcessor::escapedPropertyName(const QString &propertyName) const
{
    return sqlDataSource()->escapedPropertyName(propertyName);
}

QVariant IqOrmSqlAbstractPropertyDescriptionProcessor::excapedValue(const QVariant &value) const
{
    return sqlDataSource()->excapedValue(value);
}

QString IqOrmSqlAbstractPropertyDescriptionProcessor::escapedObjectTableName() const
{
    if (object()) {
        Q_CHECK_PTR(object()->ormMetaModel());
        Q_ASSERT(!object()->ormMetaModel()->tableName().isEmpty());
        return escapedTableName(object()->ormMetaModel()->tableName());
    }

    Q_CHECK_PTR(ormModel());
    Q_ASSERT(!ormModel()->tableName().isEmpty());

    return escapedTableName(ormModel()->tableName());
}

QString IqOrmSqlAbstractPropertyDescriptionProcessor::generateFieldStringAggColumn(const QString &tableName,
                                                                                   const QString &fieldName,
                                                                                   const QString &joinCondition,
                                                                                   const QString &asName,
                                                                                   bool *needJoin) const
{
    Q_CHECK_PTR(sqlDataSource());

    QString column;
    //Использовать будет специальный функции для различных БД
    switch (sqlDataSource()->databaseType()) {
    case IqOrmSqlDataSource::MySQL:
    case IqOrmSqlDataSource::SQLite: {
        column.append("group_concat(CAST(");
        column.append(tableName);
        column.append(".");
        column.append(fieldName);
        column.append(" AS varchar(20)), ',') AS ");
        column.append(asName);

        *needJoin = true;
        break;
    }
    case IqOrmSqlDataSource::PostgreSQL: {
        column.append("string_agg(CAST(");
        column.append(tableName);
        column.append(".");
        column.append(fieldName);
        column.append(" AS varchar(20)), ',') AS ");
        column.append(asName);

        *needJoin = true;
        break;
    }
    case IqOrmSqlDataSource::Oracle: {
        //Oracle
        column.append("listagg(CAST(");
        column.append(tableName);
        column.append(".");
        column.append(fieldName);
        column.append(" AS varchar(20)), ',') WITHIN GROUP (");
        column.append(tableName);
        column.append(".");
        column.append(escapedIdFieldName());
        column.append(") AS ");
        column.append(asName);

        *needJoin = true;
        break;
    }
    case IqOrmSqlDataSource::MSSQL: {
        //Считаем, что это MSSQL
        column.append("replace((SELECT CAST(");
        column.append(fieldName);
        column.append(" AS varchar(20)) AS 'data()' FROM ");
        column.append(tableName);
        column.append(" WHERE ");
        column.append(joinCondition);
        column.append(" for xml path('')), ' ', ',') AS ");
        column.append(asName);

        *needJoin = false;
        break;
    }
    default: {
        qWarning() << QObject::tr("In database type %0 string_agg not supported.")
                      .arg(sqlDataSource()->database().driverName());
    }
    }

    return column;
}

QSqlQuery IqOrmSqlAbstractPropertyDescriptionProcessor::execQuery(const QString &prepareString, const QList<QVariant> bindValues, bool forwardOnly, bool *ok, QString *errorText) const
{
    Q_CHECK_PTR(sqlDataSource());
    return sqlDataSource()->execQuery(prepareString, bindValues, forwardOnly, ok, errorText);
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::prepareQuery(QSqlQuery &query, const QString &prepareString, QString *errorText) const
{
    Q_CHECK_PTR(sqlDataSource());
    return sqlDataSource()->prepareQuery(query, prepareString, errorText);
}

bool IqOrmSqlAbstractPropertyDescriptionProcessor::execPreparedQuery(QSqlQuery &query, const QList<QVariant> bindValues, QString *errorText) const
{
    Q_CHECK_PTR(sqlDataSource());
    return sqlDataSource()->execPreparedQuery(query, bindValues, errorText);
}
const IqOrmMetaModel *IqOrmSqlAbstractPropertyDescriptionProcessor::ormModel() const
{
    return m_ormModel;
}

void IqOrmSqlAbstractPropertyDescriptionProcessor::setOrmModel(const IqOrmMetaModel *model)
{
    m_ormModel = model;
}

