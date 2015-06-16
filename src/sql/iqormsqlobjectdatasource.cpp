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

#include "iqormsqlobjectdatasource.h"
#include "iqormsqldatasource.h"
#include "iqormpropertydescription.h"
#include "iqormtransactioncontrol.h"

#include "iqormsqlpropertydescriptionsprocessor.h"

#include "iqormobjectprivateaccessor.h"
#include "iqormsqljoinoperation.h"

#include "iqormobject.h"
#include "iqormmetamodel.h"
#include "iqormerror.h"

#include "iqormdatasourceoperationresult.h"
#include "iqormobjectrawdata.h"

#include <QSqlQuery>
#include <QDebug>

IqOrmSqlObjectDataSource::IqOrmSqlObjectDataSource(IqOrmSqlDataSource *sqlDataSource) :
    IqOrmAbstractObjectDataSource(sqlDataSource),
    m_sqlDataSource(sqlDataSource),
    m_propertyDescriptionsProcessor(new IqOrmSqlPropertyDescriptionsProcessor(this))
{
    m_propertyDescriptionsProcessor->setSqlDataSource(m_sqlDataSource);
}

bool IqOrmSqlObjectDataSource::loadObjectFromSqlQuery(IqOrmObject *object,
                                                      const QSqlQuery &query,
                                                      QString *error)
{
    bool ok;
    IqOrmObjectRawData rawData = createRawDataForObjectFromSqlQuery(object->ormMetaModel(), query, &ok, error);
    if (!ok)
        return false;

    IqOrmPrivate::IqOrmObjectPrivateAccessor::setVaules(object, rawData);
    return true;
}

IqOrmObjectRawData IqOrmSqlObjectDataSource::createRawDataForObjectFromSqlQuery(const IqOrmMetaModel *objectOrmMetaModel,
                                                                                const QSqlQuery &query,
                                                                                bool *ok,
                                                                                QString *error)
{
    //Запрос должен быть обязательно позиционирован на нужной записи!!!

    Q_CHECK_PTR(objectOrmMetaModel);

    IqOrmObjectRawData rawData;
    rawData.objectId = query.value(0).toInt();

    int i = 0;
    foreach (const IqOrmPropertyDescription *propDescription, objectOrmMetaModel->propertyDescriptions()) {
        i++;
        Q_CHECK_PTR(propDescription);

        //Получаем значение из записи для свойства
        //Свойства в выборке должны идти в том же порядке, что и свойства в модели, со смещением на 1
        //т.к. первым свойством в выборке будет objectId, который не перечислен в модели
        //Поэтому будем использовать индексы для получения свойств с целью оптимизации скорости
        QVariant value = query.value(i);
        Q_ASSERT(value.isValid());

        //Устанавливаем значение
        bool convertOk;
        QString convertError;
        rawData.values[propDescription] = IqOrmSqlPropertyDescriptionsProcessor::convertSqlValueForProperty(propDescription, value, &convertOk, &convertError);
        if (!convertOk) {
            if (error)
                *error = convertError;
            if (ok)
                *ok = false;
            return rawData;
        }
    }

    if (error)
        *error = "";
    if (ok)
        *ok = true;

    return rawData;
}

QString IqOrmSqlObjectDataSource::generateSelectQuery(const IqOrmMetaModel *ormModel, bool distinct) const
{
    Q_CHECK_PTR(ormModel);

    Q_ASSERT(!ormModel->tableName().isEmpty());

    QString queryStr = "SELECT ";
    if (distinct)
        queryStr.append(" DISTINCT ");

    QString tableName = m_sqlDataSource->escapedTableName(ormModel->tableName());

    QStringList columns;
    columns << tableName + "." + m_sqlDataSource->escapedIdFieldName();
    columns << m_propertyDescriptionsProcessor->selectFieldNames(ormModel);

    queryStr.append(columns.join(", "));

    queryStr.append("\n    FROM ");

    queryStr.append(tableName);

    QList<IqOrmSqlJoinOperation> joinOperations = m_propertyDescriptionsProcessor->selectJoinOperations(ormModel);
    //Добавим джойны
    foreach (const IqOrmSqlJoinOperation &joinOperation, joinOperations) {
        queryStr.append("\n    " +joinOperation.toString() + " ");
    }

    return queryStr;
}

IqOrmDataSourceOperationResult IqOrmSqlObjectDataSource::loadObjectById(IqOrmObject *object, qint64 id) const
{
    IqOrmDataSourceOperationResult result;
    result.setOperation(IqOrmAbstractDataSource::Load);
    result.setObjectId(id);

    Q_CHECK_PTR(m_sqlDataSource);
    result.setDataSource(m_sqlDataSource);

    Q_CHECK_PTR(object);

    QObject *qobject = dynamic_cast<QObject*>(object);
    Q_CHECK_PTR(qobject);
    Q_ASSERT(id != -1);

    const IqOrmMetaModel *ormModel = object->ormMetaModel();
    Q_CHECK_PTR(ormModel);

    if (!m_propertyDescriptionsProcessor->selectAllowed(object, &result)) {
        resetObject(object);
        return result;
    }

    if (!m_propertyDescriptionsProcessor->preSelect(object, &result)) {
        resetObject(object);
        return result;
    }

    QString error;
    bool ok = false;

    QString queryStr = generateSelectQuery(ormModel);
    Q_ASSERT(!queryStr.isEmpty());

    QStringList wheres;
    //Добавим ид
    QString idWhere = m_sqlDataSource->escapedTableName(ormModel->tableName());
    idWhere.append(".");
    idWhere.append(m_sqlDataSource->escapedIdFieldName());
    idWhere.append("=?");
    wheres << idWhere;

    queryStr.append("\n    WHERE ");
    queryStr.append(wheres.join(" AND "));

    queryStr.append("\n    GROUP BY ");

    queryStr.append(m_sqlDataSource->escapedTableName(ormModel->tableName()));
    queryStr.append(".");
    queryStr.append(m_sqlDataSource->escapedIdFieldName());

    QVariantList bindValues;
    bindValues << id;

    QSqlQuery query = m_sqlDataSource->execQuery(queryStr, bindValues, true, &ok, &error);
    if (!ok) {
        resetObject(object);
        result.setError(error);
        return result;
    }

    if (!query.isActive()) {
        resetObject(object);
        result.setError(tr("SQL query not active."));
        return result;
    }

    //Получим первую запись
    if (!query.next()) {
        resetObject(object);
        result.setError(tr("Not found object with objectId = %0.")
                        .arg(id));
        return result;
    }

    if (!m_propertyDescriptionsProcessor->postSelect(object, &result)) {
        resetObject(object);
        return result;
    }

    //Загрузим данные в объект из первой записи
    if (!loadObjectFromSqlQuery(object, query, &error)) {
        resetObject(object);
        result.setError(error);
        return result;
    }

    return result;
}

IqOrmDataSourceOperationResult IqOrmSqlObjectDataSource::insertObject(IqOrmObject *object)
{
    IqOrmDataSourceOperationResult result;
    result.setOperation(IqOrmAbstractDataSource::Persist);

    Q_CHECK_PTR(m_sqlDataSource);
    result.setDataSource(m_sqlDataSource);

    Q_CHECK_PTR(object);
    Q_ASSERT(!IqOrmPrivate::IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(object));
    Q_ASSERT(!IqOrmPrivate::IqOrmObjectPrivateAccessor::isObjectSavedToDataSource(object));
    Q_ASSERT(object->objectId() == -1);

    const IqOrmMetaModel *ormModel = object->ormMetaModel();
    Q_CHECK_PTR (ormModel);

    if (!m_propertyDescriptionsProcessor->insertAllowed(object, &result))
        return result;

    if (!m_propertyDescriptionsProcessor->preInsert(object, &result))
        return result;

    QString tableName = m_sqlDataSource->escapedTableName(ormModel->tableName());

    QStringList fieldsList = m_propertyDescriptionsProcessor->insertFieldNames(ormModel);
    QVariantList bindValuesList = m_propertyDescriptionsProcessor->insertValues(object);

    QStringList valuesPlaceholders;
    for (int i = 0; i < fieldsList.count(); i++) {
        valuesPlaceholders.append("?");
    }

    QString queryStr = "INSERT INTO ";
    queryStr.append(tableName);
    queryStr.append("\n    ( ");
    queryStr.append(fieldsList.join(", "));
    queryStr.append(" ) ");
    queryStr.append("\n    VALUES ( ");
    queryStr.append(valuesPlaceholders.join(", "));
    queryStr.append(" ) ");

    bool ok = false;
    QString error;
    QSqlQuery query = m_sqlDataSource->execQuery(queryStr, bindValuesList, true, &ok, &error);
    result.setError(error);
    if (!ok)
        return result;

    qint64 objectId = query.lastInsertId().toInt();
    IqOrmPrivate::IqOrmObjectPrivateAccessor::setObjectId(object, objectId);

    if (!m_propertyDescriptionsProcessor->postInsert(object, &result)) {
        IqOrmPrivate::IqOrmObjectPrivateAccessor::setObjectId(object, -1);
        return result;
    }

    result.setObjectId(objectId);
    result.changes(ormModel, object->objectId())->setOperation(IqOrmAbstractDataSource::Persist);
    return result;
}

IqOrmDataSourceOperationResult IqOrmSqlObjectDataSource::updateObject(IqOrmObject *object,
                                                                      const QSet<const IqOrmPropertyDescription *> &properties)
{
    Q_CHECK_PTR(object);
    IqOrmDataSourceOperationResult result;
    result.setOperation(IqOrmAbstractDataSource::Update);
    result.setObjectId(object->objectId());

    Q_CHECK_PTR(m_sqlDataSource);
    result.setDataSource(m_sqlDataSource);

    Q_ASSERT(IqOrmPrivate::IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(object));
    Q_ASSERT((IqOrmPrivate::IqOrmObjectPrivateAccessor::isObjectSavedToDataSource(object)));
    Q_ASSERT(object->objectId() != -1);

    Q_ASSERT(!properties.isEmpty());

    const IqOrmMetaModel *ormModel = object->ormMetaModel();
    Q_CHECK_PTR(ormModel);

    if (!m_propertyDescriptionsProcessor->updateAllowed(object, properties, &result))
        return result;

    if (!m_propertyDescriptionsProcessor->preUpdate(object, properties, &result))
        return result;

    QStringList fieldsList = m_propertyDescriptionsProcessor->updateFieldNames(ormModel, properties);
    QVariantList bindValuesList = m_propertyDescriptionsProcessor->updateValues(object, properties);

    if (fieldsList.count() != 0) {
        QString tableName = m_sqlDataSource->escapedTableName(ormModel->tableName());

        QStringList sets;
        foreach (const QString &filedName, fieldsList) {
            sets << filedName + "=?";
        }

        QString queryStr = "UPDATE ";
        queryStr.append(tableName);
        queryStr.append("\n    SET ");
        queryStr.append(sets.join(", "));
        queryStr.append("\n    WHERE ");
        queryStr.append(m_sqlDataSource->escapedIdFieldName());
        queryStr.append("=?");


        bindValuesList << object->objectId();

        bool ok = false;
        QString error;
        m_sqlDataSource->execQuery(queryStr, bindValuesList, true, &ok, &error);

        if (!ok)
            return result;
    }

    if (!m_propertyDescriptionsProcessor->postUpdate(object, properties, &result))
        return result;

    result.changes(ormModel, object->objectId())->setOperation(IqOrmAbstractDataSource::Update);
    return result;
}


IqOrmDataSourceOperationResult IqOrmSqlObjectDataSource::removeObject(IqOrmObject *object)
{
    Q_CHECK_PTR(object);
    IqOrmDataSourceOperationResult result;
    result.setOperation(IqOrmAbstractDataSource::Remove);
    result.setObjectId(object->objectId());

    Q_CHECK_PTR(m_sqlDataSource);
    result.setDataSource(m_sqlDataSource);

    Q_ASSERT(IqOrmPrivate::IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(object));
    Q_ASSERT(IqOrmPrivate::IqOrmObjectPrivateAccessor::isObjectSavedToDataSource(object));
    Q_ASSERT(object->objectId() != -1);

    const IqOrmMetaModel *ormModel = object->ormMetaModel();
    Q_CHECK_PTR(ormModel);

    if (!m_propertyDescriptionsProcessor->removeAllowed(object, &result))
        return result;

    if (!m_propertyDescriptionsProcessor->preRemove(object, &result))
        return result;

    QString tableName = m_sqlDataSource->escapedTableName(ormModel->tableName());

    QString queryStr = "DELETE FROM ";
    queryStr.append(tableName);
    queryStr.append("\n    WHERE ");
    queryStr.append(m_sqlDataSource->escapedIdFieldName());
    queryStr.append("=?");

    QVariantList bindValuesList;
    bindValuesList << object->objectId();

    bool ok = false;
    QString error;
    m_sqlDataSource->execQuery(queryStr, bindValuesList, true, &ok, &error);
    result.setError(error);
    if (!ok)
        return result;

    if (!m_propertyDescriptionsProcessor->postRemove(object, &result))
        return result;

    IqOrmPrivate::IqOrmObjectPrivateAccessor::setObjectId(object, -1);

    result.changes(ormModel, object->objectId())->setOperation(IqOrmAbstractDataSource::Remove);
    return result;
}
