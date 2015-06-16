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

#include "iqormsqldirectpropertydescriptionprocessor.h"
#include "iqormdirectpropertychanges.h"

IqOrmSqlDirectPropertyDescriptionProcessor::IqOrmSqlDirectPropertyDescriptionProcessor() :
    IqOrmSqlAbstractPropertyDescriptionProcessor()
{
}

const IqOrmDirectPropertyDescription *IqOrmSqlDirectPropertyDescriptionProcessor::propertyDescription() const
{
    return qobject_cast<const IqOrmDirectPropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QVariant IqOrmSqlDirectPropertyDescriptionProcessor::propertyValue() const
{
    Q_CHECK_PTR(propertyDescription());
    return propertyDescription()->value(object());
}

QString IqOrmSqlDirectPropertyDescriptionProcessor::selectFieldName() const
{
    Q_CHECK_PTR(propertyDescription());

    QString fieldName = escapedObjectTableName();
    fieldName.append(".");
    fieldName.append(escapedFieldName(propertyDescription()->columnName()));
    fieldName.append(" AS ");
    fieldName.append(escapedPropertyName(propertyDescription()->propertyName()));

    return fieldName;
}

QString IqOrmSqlDirectPropertyDescriptionProcessor::insertFieldName() const
{
    Q_CHECK_PTR(propertyDescription());


    return escapedFieldName(propertyDescription()->columnName());
}

QString IqOrmSqlDirectPropertyDescriptionProcessor::updateFieldName() const
{
    return insertFieldName();
}

QVariant IqOrmSqlDirectPropertyDescriptionProcessor::insertValue() const
{
    return excapedValue(propertyValue());
}

QVariant IqOrmSqlDirectPropertyDescriptionProcessor::updateValue() const
{
    return insertValue();
}

QList<IqOrmSqlJoinOperation> IqOrmSqlDirectPropertyDescriptionProcessor::selectJoinOperations() const
{
    return QList<IqOrmSqlJoinOperation>();
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::selectAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::insertAllowed(IqOrmDataSourceOperationResult *result) const
{
    return checkPropertySourceValueIsValid(result);
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::updateAllowed(IqOrmDataSourceOperationResult *result) const
{
    return insertAllowed(result);
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::removeAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::preSelect(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::preInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::preUpdate(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::preRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::postSelect(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::postInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());
    Q_CHECK_PTR(propertyDescription());

    IqOrmDirectPropertyChanges *changes = result->changes(object()->ormMetaModel(), object()->objectId())->propertyChanges(propertyDescription());
    changes->setNewValue(propertyValue());

    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::postUpdate(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);

    if (propertyWasChanged()) {
        Q_CHECK_PTR(object());
        Q_CHECK_PTR(object()->ormMetaModel());
        Q_CHECK_PTR(propertyDescription());

        IqOrmDirectPropertyChanges *changes = result->changes(object()->ormMetaModel(), object()->objectId())->propertyChanges(propertyDescription());
        changes->setNewValue(propertyValue());

        result->setError("");
    }

    result->setError("");
    return true;
}

bool IqOrmSqlDirectPropertyDescriptionProcessor::postRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

QVariant IqOrmSqlDirectPropertyDescriptionProcessor::convertSqlValue(const QVariant &sqlValue, bool *ok, QString *error) const
{
    Q_CHECK_PTR(propertyDescription());

    bool createValueOk;
    QVariant result = IqOrmSqlDataSource::createValueFromSql(sqlValue,
                                                  propertyDescription()->targetStaticMetaPropery().type(),
                                                  &createValueOk);
    if (!createValueOk) {
        if (error)
            *error = QObject::tr("Error convert value %0 to %1 for property %2.")
                .arg(sqlValue.toString())
                .arg(propertyDescription()->targetStaticMetaPropery().typeName())
                .arg(propertyDescription()->propertyName());

        if (ok)
            *ok = false;
    }

    if (error)
        error->clear();
    if (ok)
        *ok = true;

    return result;
}
