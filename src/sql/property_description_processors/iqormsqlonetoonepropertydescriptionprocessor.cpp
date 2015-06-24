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

#include "iqormsqlonetoonepropertydescriptionprocessor.h"
#include "iqormonetoonepropertychanges.h"
#include "iqorminversedonetoonepropertychanges.h"
#include "iqormmetamodelprivateaccessor.h"

using namespace IqOrmPrivate;

IqOrmSqlOneToOnePropertyDescriptionProcessor::IqOrmSqlOneToOnePropertyDescriptionProcessor() :
    IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor()
{
}

const IqOrmBaseOneToOnePropertyDescription *IqOrmSqlOneToOnePropertyDescriptionProcessor::propertyDescription() const
{
    return qobject_cast<const IqOrmBaseOneToOnePropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QString IqOrmSqlOneToOnePropertyDescriptionProcessor::selectFieldName() const
{
    Q_CHECK_PTR(propertyDescription());

    QString fieldName = escapedObjectTableName();
    fieldName.append(".");
    fieldName.append(escapedJoinColumnName());
    fieldName.append(" AS ");
    fieldName.append(escapedPropertyName(propertyDescription()->propertyName()));
    return fieldName;
}

QString IqOrmSqlOneToOnePropertyDescriptionProcessor::insertFieldName() const
{
    return escapedJoinColumnName();
}

QString IqOrmSqlOneToOnePropertyDescriptionProcessor::updateFieldName() const
{
    return insertFieldName();
}

QVariant IqOrmSqlOneToOnePropertyDescriptionProcessor::insertValue() const
{
    return excapedValue(propertyValue());
}

QVariant IqOrmSqlOneToOnePropertyDescriptionProcessor::updateValue() const
{
    return insertValue();
}

QList<IqOrmSqlJoinOperation> IqOrmSqlOneToOnePropertyDescriptionProcessor::selectJoinOperations() const
{
    return QList<IqOrmSqlJoinOperation>();
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::insertAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if(!checkPropetyValueNotNull(result))
        return false;
    if(!checkPropertyValueObjectIsWasSavedToDb(result))
        return false;

    if (propertyDescription()->notNull()) {
        //Если свойство не равно нулю, то проверим, чтоб в БД не было объектов с данным ид
        //иначе после обновления их свойство станет равно нулю, что не верно
        bool ok;

        qint64 brokenObjectId = oldPropertyObjectOwnerObjectId(result, &ok);
        if (!ok)
            return false;

        if (brokenObjectId != -1) {
            //В таблице есть запись с данны ид, ошибка
            result->setError(QObject::tr("%0 with id %1 can not be NULL.")
                                     .arg(IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(object()->ormMetaModel())->className())
                                     .arg(brokenObjectId)
                                     .arg(propertyDescription()->propertyName()));
            return false;
        }
    }

    result->setError("");
    return true;
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::updateAllowed(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return insertAllowed(result);
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::removeAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::preInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if (propertyValue().isNull()) {
        result->setError("");
        return true;
    }

    bool ok;
    qint64 oldOwnerObjectId = oldPropertyObjectOwnerObjectId(result, &ok);
    if (!ok)
        return false;

    if (oldOwnerObjectId != -1) {
        QString preparedQuery = "UPDATE ";
        preparedQuery.append(escapedObjectTableName());
        preparedQuery.append("\n    SET ");
        preparedQuery.append(escapedJoinColumnName());
        preparedQuery.append("=?");
        preparedQuery.append("\n    WHERE ");
        preparedQuery.append(escapedIdFieldName());
        preparedQuery.append("=?");

        QVariantList bindValues;
        bindValues << QVariant(QVariant::LongLong);
        bindValues << oldOwnerObjectId;

        QString error;
        sqlDataSource()->execQuery(preparedQuery, bindValues, true, &ok, &error);
        if (!ok) {
            result->setError(error);
            return false;
        }

        IqOrmOneToOnePropertyChanges *changes = result->changes(object()->ormMetaModel(), oldOwnerObjectId)->propertyChanges(propertyDescription());
        changes->setOperation(IqOrmOneToOnePropertyChanges::Remove);
    }

    result->setError("");
    return true;
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::preUpdate(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return preInsert(result);
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::preRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::postInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if (propertyValue().isNull()) {
        result->setError("");
        return true;
    }

    qint64 associatedObjectId = propertyValue().toLongLong();

    if (associatedPropertyDescription()) {
        IqOrmInversedOneToOnePropertyChanges *changes = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
        changes->setOperation(IqOrmInversedOneToOnePropertyChanges::Set);
        changes->setNewObjectId(object()->objectId());
    }

    IqOrmOneToOnePropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges(propertyDescription());
    changes->setOperation(IqOrmInversedOneToOnePropertyChanges::Set);
    changes->setNewObjectId(associatedObjectId);

    result->setError("");
    return true;
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::postUpdate(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return postInsert(result);
}

bool IqOrmSqlOneToOnePropertyDescriptionProcessor::postRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if (propertyValue().isNull()) {
        result->setError("");
        return true;
    }

    qint64 associatedObjectId = propertyValue().toLongLong();

    if (associatedPropertyDescription()) {
        IqOrmInversedOneToOnePropertyChanges *changes = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
        changes->setOperation(IqOrmInversedOneToOnePropertyChanges::Remove);
        changes->setNewObjectId(object()->objectId());
    }

    result->setError("");
    return true;
}

const IqOrmBaseInversedOneToOnePropertyDescription *IqOrmSqlOneToOnePropertyDescriptionProcessor::associatedPropertyDescription() const
{
    Q_CHECK_PTR(propertyDescription());
    return qobject_cast<const IqOrmBaseInversedOneToOnePropertyDescription *>(propertyDescription()->inversedPropertyDescription());
}

QString IqOrmSqlOneToOnePropertyDescriptionProcessor::escapedJoinColumnName() const
{
    Q_CHECK_PTR(propertyDescription());

    return escapedFieldName(propertyDescription()->joinColumnName());
}

qint64 IqOrmSqlOneToOnePropertyDescriptionProcessor::oldPropertyObjectOwnerObjectId(IqOrmDataSourceOperationResult *result,
                                                                      bool *ok) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(ok);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());

    if (propertyValue().isNull()) {
        result->setError("");
        *ok = true;
        return -1;
    }

    qint64 objectId = propertyValue().toLongLong();

    QString preparedQuery = "SELECT ";
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append("\n    FROM ");
    preparedQuery.append(escapedObjectTableName());
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinColumnName()));
    preparedQuery.append("=?");

    QVariantList bindValues;
    bindValues << objectId;

    QString error;
    QSqlQuery resultQuery = execQuery(preparedQuery, bindValues, true, ok, &error);
    if (!(*ok)) {
        result->setError(error);
        return -1;
    }

    if (resultQuery.size() == 0) {
        result->setError("");
        return -1;
    }

    resultQuery.next();

    result->setError("");
    *ok = true;
    return resultQuery.value(0).toLongLong();
}
