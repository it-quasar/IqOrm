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
#include "iqormdirectpropertydescription.h"
#include "iqormerror.h"
#include "iqormobject.h"
#include <QDebug>
#include <QSqlQuery>
#include <QTime>
#include "iqormobjectprivateaccessor.h"
#include "iqormtransactioncontrol.h"
#include "iqormmodelprivateaccessor.h"
#include "iqormdatasourceoperationresult.h"


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
    QTime time;
    time.start();

    IqOrmDataSourceOperationResult result;
    Q_CHECK_PTR(model);
    Q_CHECK_PTR(m_sqlDataSource);

    const IqOrmMetaModel *ormModel = model->childsOrmMetaModel();
    Q_CHECK_PTR(ormModel);

    bool ok = false;
    QString error;
    //Для генерации запроса используем генератор запросов объекта
    QString queryStr = m_sqlDataSource->objectDataSource()->generateSelectQuery(ormModel);
    Q_ASSERT(!queryStr.isEmpty());

    QList<QVariant> bindValues;

    //Сформируем WHERE
    QString whereStr;
    //Добавим список фильтров
    whereStr = filterString(ormModel, model->filters(), &bindValues);

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
        orderStr = "\n    ORDER BY " + m_sqlDataSource->escapedIdFieldName() + " ASC ";
        break;
    case Desc:
        orderStr = "\n    ORDER BY " + m_sqlDataSource->escapedIdFieldName() + " DESC ";
        break;
    }

    //Добавим WHERE
    if (!whereStr.isEmpty())
        queryStr.append(QString("\n    WHERE %0").arg(whereStr));

    queryStr.append("\n    GROUP BY ");
    queryStr.append(m_sqlDataSource->escapedTableName(ormModel->tableName()));
    queryStr.append(".");
    queryStr.append(m_sqlDataSource->escapedIdFieldName());

    //Добавим сортировку
    queryStr.append(orderStr);
    //Добавми лими
    queryStr.append(limitStr);
    //Добавим оффсет
    queryStr.append(offsetStr);

    qDebug() << "Prepare " << time.elapsed() << " msec";

    QSqlQuery query = m_sqlDataSource->execQuery(queryStr, bindValues, true, &ok, &error);

    if (!ok) {
        model->clear();
        result.setError(error);
        return result;
    }

    qDebug() << "Exec " << time.elapsed() << " msec";

    QList<IqOrmObjectRawData> modelValues;
    //Пройдемся по всем записям
    while (query.next()) {
        IqOrmObjectRawData rawData = IqOrmSqlObjectDataSource::createRawDataForObjectFromSqlQuery(ormModel, query, &ok);

        Q_ASSERT(ok);
        modelValues << rawData;
    }

    qDebug() << "Walk " << time.elapsed() << " msec";

    IqOrmPrivate::IqOrmModelPrivateAccessor::setObjectValues(model, modelValues);

    qDebug() << "End " << time.elapsed() << " msec";

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

//QList<qint64> IqOrmSqlObjectsModelDataSource::findObjects(const IqOrmModel *ormModel,
//        const IqOrmAbstractFilter *filter,
//        const qint64 limit,
//        const qint64 offset,
//        bool *ok,
//        QString *errorMessage) const
//{
//    if (!m_sqlDataSource) {
//        if (errorMessage)
//            *errorMessage = tr("Not found valid SQL Data Source.");
//        if (ok)
//            *ok = false;
//        return QList<qint64>();
//    }


//    QSqlDriver* driver = m_sqlDataSource->database().driver();

//    QString queryStr = "SELECT id FROM ";

//    if (!ormModel) {
//        if (errorMessage)
//            *errorMessage =tr("Not found valid child objects ORM model.");
//        if (ok)
//            *ok = false;
//        return QList<qint64>();
//    }

//    QString tableName = driver->escapeIdentifier(ormModel->tableName(), QSqlDriver::TableName);

//    if (tableName.isEmpty()) {
//        if (errorMessage)
//            *errorMessage = tr("Table name for child objects ORM Model not set. Use IqOrmModel::setTableName() first.");
//        if (ok)
//            *ok = false;
//        return QList<qint64>();
//    }

//    queryStr.append(tableName);

//    QStringList refTableNamesList = referencingTableNames(ormModel, filter);
//    QString refTableNamesString = refTableNamesList.join(", ");
//    if (!refTableNamesString.isEmpty())
//        queryStr.append(QString(", %0").arg(refTableNamesString));

//    QList<QVariant> bindValues;
//    QString whereStr;
//    whereStr = filterString(ormModel, filter, &bindValues);
//    if (!whereStr.isEmpty())
//        queryStr.append(QString(" WHERE %0").arg(whereStr));

//    queryStr.append(" ORDER BY " + driver->escapeIdentifier("id", QSqlDriver::FieldName));

//    if (limit != -1)
//        queryStr.append(QString(" LIMIT %0 ").arg(limit));

//    if (offset > 0)
//        queryStr.append(QString(" OFFSET %0 ").arg(offset));

//    bool queryOk = false;
//    QSqlQuery query = m_sqlDataSource->execQuery(queryStr, bindValues, true, &queryOk, errorMessage);

//    if (!queryOk) {
//        if (ok)
//            *ok = false;
//        return QList<qint64>();
//    }

//    QList<qint64> result;

//    while (query.next()) {
//        result << query.value(0).toLongLong();
//    }

//    if (ok)
//        *ok = true;

//    return result;
//}

QString IqOrmSqlModelDataSource::filterString(const IqOrmMetaModel *ormModel,
                                                     const IqOrmAbstractFilter *filter,
                                                     QVariantList *bindValues) const
{
    Q_CHECK_PTR (ormModel);
    Q_CHECK_PTR(bindValues);

    if (!filter)
        return "";

    //Если это просто фильтр
    const IqOrmFilter *simpleFilter = qobject_cast<const IqOrmFilter *>(filter);
    if (simpleFilter) {
        if (simpleFilter->operation() == IqOrmFilter::None)
            return "";

        QString referencingString = "";
        QString columnName = "";

        //Получим свойство модели
        const IqOrmPropertyDescription *propDescription = ormModel->propertyDescription(simpleFilter->property());
        if (propDescription) {
            switch (propDescription->mappedType()) {
            case IqOrmPropertyDescription::Direct: {
                const IqOrmDirectPropertyDescription *directPropertyDescription = qobject_cast<const IqOrmDirectPropertyDescription *>(propDescription);
                Q_CHECK_PTR(directPropertyDescription);

                columnName = m_sqlDataSource->escapedTableName(ormModel->tableName());
                columnName.append(".");
                columnName.append(m_sqlDataSource->escapedFieldName(directPropertyDescription->columnName()));

                break;
            }
            }
        } else if (simpleFilter->property() == QStringLiteral("objectId")) {
            //Отдельно обработаем свойство "objectId"
            columnName = m_sqlDataSource->escapedTableName(ormModel->tableName());
            columnName.append(".");
            columnName.append(m_sqlDataSource->escapedIdFieldName());
        }

        Q_ASSERT(!columnName.isEmpty());

        QString filterString = "";

        QVariant::Type propertyType;
        if (propDescription) {
            propertyType = propDescription->targetStaticMetaPropery().type();
        } else if (simpleFilter->property() == QStringLiteral("objectId")) {
            propertyType = QVariant::LongLong;
        }

        //В зависимости от типа свойства модели
        switch (propertyType) {
        case QVariant::Invalid:
            return "";
            break;

            //Строка
        case QVariant::String: {
            QString caseSesitivityColumnName = "";
            QString caseSesitivityBindParam = "";
            if (simpleFilter->caseSensitivity()) {
                caseSesitivityColumnName = columnName;
                caseSesitivityBindParam = "?";
            } else {
                caseSesitivityColumnName = QString("LOWER(%0)").arg(columnName);
                caseSesitivityBindParam = "LOWER(?)";
            }

            switch (simpleFilter->operation()) {
            case IqOrmFilter::NotEquals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 <> %1")
                               .arg(caseSesitivityColumnName)
                               .arg(caseSesitivityBindParam);
                break;
            case IqOrmFilter::Equals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 = %1")
                               .arg(caseSesitivityColumnName)
                               .arg(caseSesitivityBindParam);
                break;
            case IqOrmFilter::StartsWith:
                bindValues->append(simpleFilter->value().toString() + "%");
                filterString = QString("%0 LIKE %1")
                               .arg(caseSesitivityColumnName)
                               .arg(caseSesitivityBindParam);
                break;
            case IqOrmFilter::Contains:
                bindValues->append("%" + simpleFilter->value().toString() + "%");
                filterString = QString("%0 LIKE %1")
                               .arg(caseSesitivityColumnName)
                               .arg(caseSesitivityBindParam);
                break;
            case IqOrmFilter::EndsWith:
                bindValues->append("%" + simpleFilter->value().toString());
                filterString = QString("%0 LIKE %1")
                               .arg(caseSesitivityColumnName)
                               .arg(caseSesitivityBindParam);
                break;
            default:
                filterString = "";
                break;
            }
            break;
        }

        //Числа
        case QVariant::Char:
        case QVariant::UInt:
        case QVariant::Int:
        case QVariant::ULongLong:
        case QVariant::LongLong:
        case QVariant::Double: {
            switch (simpleFilter->operation()) {
            case IqOrmFilter::Equals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 = ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::NotEquals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 <> ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::GreaterThan:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 > ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::GreaterOrEquals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 >= ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::LessThan:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 < ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::LessOrEquals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 <= ?")
                               .arg(columnName);
                break;
            default:
                filterString = "";
                break;
            }
            break;
        }
        default:
            //Ведем себя как с неизвестным типом
            switch (simpleFilter->operation()) {
            case IqOrmFilter::Equals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 = ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::NotEquals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 <> ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::GreaterThan:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 > ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::GreaterOrEquals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 >= ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::LessThan:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 < ?")
                               .arg(columnName);
                break;
            case IqOrmFilter::LessOrEquals:
                bindValues->append(simpleFilter->value());
                filterString = QString("%0 <= ?")
                               .arg(columnName);
                break;
            default:
                filterString = "";
                break;
            }

            break;
        }

        return QString("%0 %1")
               .arg(filterString)
               .arg(referencingString);
    }


    //Если это групповой фильтр
    const IqOrmGroupFilter *groupFilter = qobject_cast<const IqOrmGroupFilter*>(filter);
    if (groupFilter) {
        QStringList resultList;
        foreach (IqOrmAbstractFilter *childFilter, groupFilter->toList()) {
            if (!childFilter)
                continue;

            resultList << filterString(ormModel, childFilter, bindValues);
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

    return "";
}

//QStringList IqOrmSqlObjectsModelDataSource::referencingTableNames(const IqOrmModel *ormModel,
//                                                                  const IqOrmAbstractFilter *filter) const
//{
//    if (!ormModel)
//        return QStringList();

//    if (!filter)
//        return QStringList();

//    QSqlDriver* sqlDriver = m_sqlDataSource->database().driver();
//    if (!sqlDriver)
//        return QStringList();

//    //Если это просто фильтр
//    const IqOrmFilter *simpleFilter = qobject_cast<const IqOrmFilter *>(filter);
//    if (simpleFilter) {
//        if (simpleFilter->operation() == IqOrmFilter::None)
//            return QStringList();

//        //Получим свойство модели
//        IqOrmPropertyDescription *propDescription = ormModel->propertyDescription(simpleFilter->property());
//        if (!propDescription)
//            return QStringList();

//        switch (propDescription->mappedType()) {
//        case IqOrmPropertyDescription::Direct: {
//            return QStringList();
//            break;
//        }
//        }
//    }

//    //Если это групповой фильтр
//    const IqOrmGroupFilter *groupFilter = qobject_cast<const IqOrmGroupFilter*>(filter);
//    if (groupFilter) {
//        QStringList resultList;
//        foreach (IqOrmAbstractFilter *childFilter, groupFilter->toList()) {
//            if (!childFilter)
//                continue;

//            resultList << referencingTableNames(ormModel, childFilter);
//        }

//        return resultList;
//    }

//    return QStringList();
//}
