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

#include "iqormsqlmodeldatasource.h"
#include "iqormsqlobjectdatasource.h"
#include "iqormfilter.h"
#include "iqormgroupfilter.h"
#include "iqormabstractfilter.h"
#include "iqormmetamodel.h"
#include "iqormbasemodel.h"
#include "iqormsqldatasource.h"
#include "iqormerror.h"
#include "iqormobject.h"
#include <QDebug>
#include <QSqlQuery>
#include <QTime>
#include "iqormobjectprivateaccessor.h"
#include "iqormtransactioncontrol.h"
#include "iqormmodelprivateaccessor.h"
#include "iqormdatasourceoperationresult.h"
#include "iqormdirectpropertydescription.h"
#include "iqormmanytoonepropertydescription.h"


IqOrmSqlModelDataSource::IqOrmSqlModelDataSource(IqOrmSqlDataSource *sqlDataSource) :
    IqOrmAbstractModelDataSource(sqlDataSource),
    m_sqlDataSource(sqlDataSource)
{
}

IqOrmDataSourceOperationResult IqOrmSqlModelDataSource::loadModel(IqOrmBaseModel *model,
                                                                  qint64 limit,
                                                                  qint64 offset,
                                                                  OrderBy orderBy)
{
#if defined(IQORM_DEBUG_MODE)
    QTime elaplesTime;
    elaplesTime.start();
    qDebug("Ok. Start load model IqOrmModel<%s>.",
           model->childsOrmMetaModel()->targetStaticMetaObject()->className());
#endif

    IqOrmDataSourceOperationResult result;
    Q_CHECK_PTR(model);
    Q_CHECK_PTR(m_sqlDataSource);

    const IqOrmMetaModel *ormModel = model->childsOrmMetaModel();
    Q_CHECK_PTR(ormModel);

    bool ok = false;
    QString error;
    //Для генерации запроса используем генератор запросов объекта
    QString queryStr = m_sqlDataSource->objectDataSource()->generateSelectQuery(ormModel, true);
    Q_ASSERT(!queryStr.isEmpty());

    QList<QVariant> bindValues;

    //Сформируем WHERE
    QString whereStr;
    //Добавим список фильтров
    whereStr = filterString(ormModel, model->filters(), &bindValues, &ok, &error);

#if defined(IQORM_DEBUG_MODE)
    if (ok)
        qDebug("Ok. Create filter for IqOrmModel<%s> completed in %d msec.",
               model->childsOrmMetaModel()->targetStaticMetaObject()->className(),
               elaplesTime.elapsed());
    else
        qWarning("Error. Error on create filter for IqOrmModel<%s> in %d msec. Error: %s",
                 model->childsOrmMetaModel()->targetStaticMetaObject()->className(),
                 elaplesTime.elapsed(),
                 error.toLocal8Bit().constData());
#endif

    if (!ok) {
        model->clear();
        result.setError(error);
        return result;
    }

    //Сформируем лимиты
    QString limitStr;
    if (limit != -1) {
        //В зависимости от типа БД
        switch (m_sqlDataSource->databaseType()) {
        case IqOrmSqlDataSource::MySQL:
        case IqOrmSqlDataSource::PostgreSQL:
        case IqOrmSqlDataSource::SQLite: {
            limitStr = QString("\n    LIMIT %0 ").arg(limit);
            break;
        }
        case IqOrmSqlDataSource::Oracle: {
            whereStr.append("\n    ROWNUM <= %0 ").arg(limit);
            break;
        }
        case IqOrmSqlDataSource::MSSQL: {
            queryStr.insert(6, QString(" TOP %0 ").arg(limit));
            break;
        }
        default: {
            qWarning() << tr("In database type %0 limits not supported.")
                          .arg(m_sqlDataSource->database().driverName());
            break;
        }
        }
    }

    //Сформируем оффсет
    QString offsetStr;
    if (offset > 0) {
        //В зависимости от типа БД
        switch (m_sqlDataSource->databaseType()) {
        case IqOrmSqlDataSource::MySQL:
        case IqOrmSqlDataSource::PostgreSQL:
        case IqOrmSqlDataSource::SQLite: {
            offsetStr = QString("\n    OFFSET %0 ").arg(offset);
            break;
        }
        case IqOrmSqlDataSource::Oracle:
        case IqOrmSqlDataSource::MSSQL: {
            offsetStr = QString("\n    OFFSET %0 Rows").arg(offset);
            break;
        }
        default: {
            qWarning() << tr("In database type %0 offset not supported.")
                          .arg(m_sqlDataSource->database().driverName());
            break;
        }
        }
    }

    //Сформируем сотрировку
    QString orderStr;
    switch (orderBy) {
    case Asc:
        orderStr = "\n    ORDER BY " + m_sqlDataSource->escapedTableName(ormModel->tableName()) + "." + m_sqlDataSource->escapedIdFieldName() + " ASC ";
        break;
    case Desc:
        orderStr = "\n    ORDER BY " + m_sqlDataSource->escapedTableName(ormModel->tableName()) + "." + m_sqlDataSource->escapedIdFieldName() + " DESC ";
        break;
    }

    //Добавим WHERE
    if (!whereStr.isEmpty())
        queryStr.append(QString("\n    WHERE %0").arg(whereStr));

    //Временно удалим GROUP BY. Надо вспомнить зачем она тут вообще
//    queryStr.append("\n    GROUP BY ");
//    queryStr.append(m_sqlDataSource->escapedTableName(ormModel->tableName()));
//    queryStr.append(".");
//    queryStr.append(m_sqlDataSource->escapedIdFieldName());

    //Добавим сортировку
    queryStr.append(orderStr);
    //Добавми лими
    queryStr.append(limitStr);
    //Добавим оффсет
    queryStr.append(offsetStr);

#if defined(IQORM_DEBUG_MODE)
    qDebug("Ok. Query for load model IqOrmModel<%s> prepared in %d msec.",
           model->childsOrmMetaModel()->targetStaticMetaObject()->className(),
           elaplesTime.elapsed());
#endif

    QSqlQuery query = m_sqlDataSource->execQuery(queryStr, bindValues, true, &ok, &error);

#if defined(IQORM_DEBUG_MODE)
    if (ok)
        qDebug("Ok. Query for load model IqOrmModel<%s> executed in %d msec.",
               model->childsOrmMetaModel()->targetStaticMetaObject()->className(),
               elaplesTime.elapsed());
    else
        qWarning("Error. Query for load model IqOrmModel<%s> executed in %d msec. Error: \"%s\".",
               model->childsOrmMetaModel()->targetStaticMetaObject()->className(),
               elaplesTime.elapsed(),
               error.toLocal8Bit().constData());
#endif

    if (!ok) {
        model->clear();
        result.setError(error);
        return result;
    }

    QList<IqOrmObjectRawData> modelValues;
    //Пройдемся по всем записям
    while (query.next()) {
        IqOrmObjectRawData rawData = IqOrmSqlObjectDataSource::createRawDataForObjectFromSqlQuery(ormModel, query, &ok);

        Q_ASSERT(ok);
        modelValues << rawData;
    }

#if defined(IQORM_DEBUG_MODE)
    qDebug("Ok. Walking on query for load model IqOrmModel<%s> fineshed in %d msec.",
           model->childsOrmMetaModel()->targetStaticMetaObject()->className(),
           elaplesTime.elapsed());
#endif

    IqOrmPrivate::IqOrmModelPrivateAccessor::setObjectValues(model, modelValues);

#if defined(IQORM_DEBUG_MODE)
    qDebug("Ok. Load model IqOrmModel<%s> fineshed in %d msec.",
           model->childsOrmMetaModel()->targetStaticMetaObject()->className(),
           elaplesTime.elapsed());
#endif

    return result;
}

IqOrmDataSourceOperationResult IqOrmSqlModelDataSource::truncateModel(const IqOrmMetaModel *ormModel)
{
    IqOrmDataSourceOperationResult result;
    Q_CHECK_PTR(m_sqlDataSource);
    Q_CHECK_PTR(ormModel);

    QString queryStr = "TRUNCATE TABLE ";
    queryStr.append(m_sqlDataSource->escapedTableName(ormModel->tableName()));

    bool ok;
    QString error;
    m_sqlDataSource->execQuery(queryStr, &ok, &error);
    if (!ok) {
        result.setError(error);
        return result;
    }

    return result;
}

QString IqOrmSqlModelDataSource::filterString(const IqOrmMetaModel *ormModel,
                                              const IqOrmAbstractFilter *filter,
                                              QVariantList *bindValues,
                                              bool *ok,
                                              QString *errorString) const
{
    Q_CHECK_PTR (ormModel);
    //Не проверяме фильтр, т.к. он может быть равен нулю
    Q_CHECK_PTR(bindValues);
    Q_CHECK_PTR(ok);
    Q_CHECK_PTR(errorString);

    if (!filter) {
        *ok = true;
        return "";
    }

    //Если это просто фильтр
    const IqOrmFilter *directFilter = qobject_cast<const IqOrmFilter *>(filter);
    if (directFilter) {
        return simpleFilterString(ormModel, directFilter, bindValues, ok, errorString);
    }

    //Если это групповой фильтр
    const IqOrmGroupFilter *groupFilter = qobject_cast<const IqOrmGroupFilter*>(filter);
    if (groupFilter) {
        QStringList resultList;
        foreach (IqOrmAbstractFilter *childFilter, groupFilter->toList()) {
            if (!childFilter)
                continue;

            resultList << filterString(ormModel, childFilter, bindValues, ok, errorString);
            if (!(*ok))
                return "";
        }

        if (!resultList.isEmpty()) {
            //В зависимости от типа фильтра
            if (groupFilter->type() == IqOrmGroupFilter::Or) {
                return QString("( %0 )").arg(resultList.join(" OR "));
            } else {
                return QString("( %0 )").arg(resultList.join(" AND "));
            }
        }
    }

    *ok = true;
    return "";
}

QString IqOrmSqlModelDataSource::simpleFilterString(const IqOrmMetaModel *ormModel,
                                                    const IqOrmFilter *filter,
                                                    QVariantList *bindValues,
                                                    bool *ok,
                                                    QString *errorString) const
{
    Q_CHECK_PTR(ormModel);
    Q_CHECK_PTR(filter);
    Q_CHECK_PTR(bindValues);
    Q_CHECK_PTR(ok);
    Q_CHECK_PTR(errorString);

    if (filter->condition() == IqOrmFilter::None) {
        *ok = true;
        return "";
    }

    //Получим свойство модели
    const IqOrmPropertyDescription *propDescription = ormModel->propertyDescription(filter->property());
    if (propDescription) {
        switch (propDescription->mappedType()) {
        case IqOrmPropertyDescription::Direct: {
            const IqOrmDirectPropertyDescription *directPropertyDescription = qobject_cast<const IqOrmDirectPropertyDescription *>(propDescription);
            Q_CHECK_PTR(directPropertyDescription);

            return filterStringForDirectProperty(ormModel, filter, directPropertyDescription, bindValues, ok, errorString);
            break;
        }
        case IqOrmPropertyDescription::ManyToOne: {
            const IqOrmBaseManyToOnePropertyDescription *manyToOnePropertyDescription = qobject_cast<const IqOrmBaseManyToOnePropertyDescription *>(propDescription);
            Q_CHECK_PTR(manyToOnePropertyDescription);

            return filterStringForManyToOneProperty(ormModel, filter, manyToOnePropertyDescription, bindValues, ok, errorString);
            break;
        }
        }
    } else if (filter->property() == QStringLiteral("objectId")) {
        //Отдельно обработаем свойство "objectId"
        return filterStringForDirectProperty(ormModel, filter, Q_NULLPTR, bindValues, ok, errorString);
    } else {
        *ok = false;
        *errorString = tr ("Property \"%0\" of IqOrmFilter not found in class \"%1\".")
                .arg(filter->property())
                .arg(ormModel->targetStaticMetaObject()->className());
        return "";
    }

    *ok = true;
    return "";
}

QString IqOrmSqlModelDataSource::filterStringForDirectProperty(const IqOrmMetaModel *ormModel,
                                                               const IqOrmFilter *filter,
                                                               const IqOrmDirectPropertyDescription *propertyDesctiption,
                                                               QVariantList *bindValues,
                                                               bool *ok,
                                                               QString *errorString) const
{
    Q_CHECK_PTR(ormModel);
    //Не проверяем filter, т.к. он может быть нулем (если свойство objectId)
    Q_CHECK_PTR(propertyDesctiption);
    Q_CHECK_PTR(ok);
    Q_CHECK_PTR(errorString);

    QString columnName = "";

    //Получим свойство модели
    if (propertyDesctiption) {
        columnName = m_sqlDataSource->escapedTableName(ormModel->tableName());
        columnName.append(".");
        columnName.append(m_sqlDataSource->escapedFieldName(propertyDesctiption->columnName()));
    } else if (filter->property() == QStringLiteral("objectId")) {
        //Отдельно обработаем свойство "objectId"
        columnName = m_sqlDataSource->escapedTableName(ormModel->tableName());
        columnName.append(".");
        columnName.append(m_sqlDataSource->escapedIdFieldName());
    }

    Q_ASSERT(!columnName.isEmpty());

    QString filterString = "";

    QVariant::Type propertyType;
    if (propertyDesctiption) {
        propertyType = propertyDesctiption->targetStaticMetaPropery().type();
    } else if (filter->property() == QStringLiteral("objectId")) {
        propertyType = QVariant::LongLong;
    }

    //В зависимости от типа свойства модели
    switch (propertyType) {
    case QVariant::Invalid:
        *ok = true;
        return "";
        break;

        //Строка
    case QVariant::String: {
        QString caseSesitivityColumnName = "";
        QString caseSesitivityBindParam = "";
        if (filter->caseSensitivity()) {
            caseSesitivityColumnName = columnName;
            caseSesitivityBindParam = "?";
        } else {
            caseSesitivityColumnName = QString("LOWER(%0)").arg(columnName);
            caseSesitivityBindParam = "LOWER(?)";
        }

        switch (filter->condition()) {
        case IqOrmFilter::NotEquals:
            bindValues->append(filter->value());
            filterString = QString("%0 <> %1")
                    .arg(caseSesitivityColumnName)
                    .arg(caseSesitivityBindParam);
            break;
        case IqOrmFilter::Equals:
            bindValues->append(filter->value());
            filterString = QString("%0 = %1")
                    .arg(caseSesitivityColumnName)
                    .arg(caseSesitivityBindParam);
            break;
        case IqOrmFilter::StartsWith:
            bindValues->append(filter->value().toString() + "%");
            filterString = QString("%0 LIKE %1")
                    .arg(caseSesitivityColumnName)
                    .arg(caseSesitivityBindParam);
            break;
        case IqOrmFilter::Contains:
            bindValues->append("%" + filter->value().toString() + "%");
            filterString = QString("%0 LIKE %1")
                    .arg(caseSesitivityColumnName)
                    .arg(caseSesitivityBindParam);
            break;
        case IqOrmFilter::EndsWith:
            bindValues->append("%" + filter->value().toString());
            filterString = QString("%0 LIKE %1")
                    .arg(caseSesitivityColumnName)
                    .arg(caseSesitivityBindParam);
            break;
        default:
            int enumIndex = IqOrmFilter::staticMetaObject.indexOfEnumerator("Condition");
            Q_ASSERT(enumIndex != -1);
            QMetaEnum metaEnum = IqOrmFilter::staticMetaObject.enumerator(enumIndex);
            const char *conditionName = metaEnum.key(filter->condition());
            *ok = false;
            *errorString = tr("IqOrmFilter for QString property \"%0\" not support \"%1\" condition.")
                    .arg(propertyDesctiption->propertyName())
                    .arg(conditionName);
            return "";
            break;
        }
        break;
    }

    default:
        //Ведем себя как с неизвестным типом
        bindValues->append(filter->value());
        return columnCondition(columnName, propertyDesctiption->propertyName(), filter->condition(), ok, errorString);
        break;
    }

    *ok = true;
    return filterString;
}

QString IqOrmSqlModelDataSource::filterStringForManyToOneProperty(const IqOrmMetaModel *ormModel,
                                                                  const IqOrmFilter *filter,
                                                                  const IqOrmBaseManyToOnePropertyDescription *propertyDesctiption,
                                                                  QVariantList *bindValues,
                                                                  bool *ok,
                                                                  QString *errorString) const
{
    Q_CHECK_PTR(ormModel);
    Q_CHECK_PTR(filter);
    Q_CHECK_PTR(propertyDesctiption);
    Q_CHECK_PTR(ok);
    Q_CHECK_PTR(errorString);

    QString columnName;
    columnName = m_sqlDataSource->escapedTableName(ormModel->tableName());
    columnName.append(".");
    columnName.append(m_sqlDataSource->escapedFieldName(propertyDesctiption->joinColumnName()));

    Q_ASSERT(!columnName.isEmpty());

    qint64 objectId = -1;
    QVariant filterValue = filter->value();
    if (filterValue.canConvert<QObject *>()) {
        QObject *qobject = filterValue.value<QObject *>();
        IqOrmObject *ormObject = dynamic_cast<IqOrmObject *>(qobject);
        if (ormObject)
            objectId = ormObject->objectId();
        else {
            *ok = false;
            *errorString = tr("IqOrmFilter for Many To One property \"%0\" support only \"IqOrmObject *\" as value.")
                    .arg(propertyDesctiption->propertyName());
            return "";
        }
    }

    if (objectId == -1) {
        *ok = false;
        *errorString = tr("IqOrmFilter for Many To One property \"%0\" must have persisted \"IqOrmObject *\" as value.")
                .arg(propertyDesctiption->propertyName());
        return "";
    }

    bindValues->append(objectId);

    QString filterString;
    switch (filter->condition()) {
    case IqOrmFilter::Equals:
        filterString = QString("%0 = ?")
                .arg(columnName);
        break;
    case IqOrmFilter::NotEquals:
        filterString = QString("%0 <> ?")
                .arg(columnName);
        break;
    default:
        int enumIndex = IqOrmFilter::staticMetaObject.indexOfEnumerator("Condition");
        Q_ASSERT(enumIndex != -1);
        QMetaEnum metaEnum = IqOrmFilter::staticMetaObject.enumerator(enumIndex);
        const char *conditionName = metaEnum.key(filter->condition());
        *ok = false;
        *errorString = tr("IqOrmFilter for Many To One property \"%0\" not support \"%1\" condition.")
                .arg(propertyDesctiption->propertyName())
                .arg(conditionName);
        return "";
        break;
    }

    *ok = true;
    return filterString;
}

QString IqOrmSqlModelDataSource::columnCondition(const QString &columnName,
                                                 const QString &propertyName,
                                                 IqOrmFilter::Condition condition,
                                                 bool *ok,
                                                 QString *errorString) const
{
    QString conditionString;
    switch (condition) {
    case IqOrmFilter::Equals:
        conditionString = QString("%0 = ?")
                .arg(columnName);
        break;
    case IqOrmFilter::NotEquals:
        conditionString = QString("%0 <> ?")
                .arg(columnName);
        break;
    case IqOrmFilter::GreaterThan:
        conditionString = QString("%0 > ?")
                .arg(columnName);
        break;
    case IqOrmFilter::GreaterOrEquals:
        conditionString = QString("%0 >= ?")
                .arg(columnName);
        break;
    case IqOrmFilter::LessThan:
        conditionString = QString("%0 < ?")
                .arg(columnName);
        break;
    case IqOrmFilter::LessOrEquals:
        conditionString = QString("%0 <= ?")
                .arg(columnName);
        break;
    default:
        int enumIndex = IqOrmFilter::staticMetaObject.indexOfEnumerator("Condition");
        Q_ASSERT(enumIndex != -1);
        QMetaEnum metaEnum = IqOrmFilter::staticMetaObject.enumerator(enumIndex);
        const char *conditionName = metaEnum.key(condition);
        *ok = false;
        *errorString = tr("IqOrmFilter for not QString property \"%0\" not support \"%1\" condition.")
                .arg(propertyName)
                .arg(conditionName);
        return "";
        break;
    }

    *ok = true;
    return conditionString;
}
