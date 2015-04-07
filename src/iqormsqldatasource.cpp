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

#include "iqormsqldatasource.h"
#include "iqormobject.h"
#include "iqormmetamodel.h"
#include "iqormpropertydescription.h"
#include <QMetaProperty>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QTime>

IqOrmSqlDataSource::IqOrmSqlDataSource(QObject *parent) :
    IqOrmAbstractDataSource(parent),
    m_escapedIdFieldName(""),
    m_sqlDriver(Q_NULLPTR),
    m_objectDataSource(new IqOrmSqlObjectDataSource(this)),
    m_objectsModelDataSource(new IqOrmSqlObjectsModelDataSource(this)),
    m_transactionIsOpen(false),
    m_tracerLog(new IqOrmSqlOperationTracerLog(this))
{
}

QVariant IqOrmSqlDataSource::excapedValue(const QVariant &value) const
{
    switch (static_cast<QMetaType::Type>(value.type())) {
    case QMetaType::UChar:
    case QMetaType::Char:
        return value.toInt();
    default:
        break;
    }

    return value;
}

IqOrmSqlObjectDataSource * IqOrmSqlDataSource::objectDataSource() const
{
    return m_objectDataSource;
}

IqOrmSqlObjectsModelDataSource * IqOrmSqlDataSource::objectsModelDataSource() const
{
    return m_objectsModelDataSource;
}

QSqlQuery IqOrmSqlDataSource::execQuery(const QString &query, bool *ok, QString *errorText) const
{
    Q_ASSERT(m_database.isValid());

    QTime elapsedTime;
    elapsedTime.start();

    if (ok)
        *ok = false;

    //Готовим запрос
    QSqlQuery sqlQuery (m_database);

    if (!openDB()) {
        QString error = tr("Can not open database \"%0\" on host \"%1:%2\" as \"%3\" with password \"*********\".")
                .arg(m_database.databaseName())
                .arg(m_database.hostName())
                .arg(m_database.port())
                .arg(m_database.userName());
        if (errorText)
            *errorText = error;

        tracerLog()->emitExecQuery(query, elapsedTime.elapsed(), false, error);
        return sqlQuery;
    }

    bool result = sqlQuery.exec(query);
    if (ok)
        *ok = result;
    if (errorText)
        *errorText = sqlQuery.lastError().text();

    tracerLog()->emitExecQuery(query, elapsedTime.elapsed(), result, sqlQuery.lastError().text());
    return sqlQuery;
}

bool IqOrmSqlDataSource::openTransaction()
{
    Q_ASSERT(m_database.isValid());

    m_transactionMutex.lock();
    Q_ASSERT_X(!m_transactionIsOpen, Q_FUNC_INFO, "Transaction already open.");

    if (!openDB()) {
        QString error = tr("Can not open database \"%0\" on host \"%1:%2\" as \"%3\" with password \"*********\".")
                .arg(m_database.databaseName())
                .arg(m_database.hostName())
                .arg(m_database.port())
                .arg(m_database.userName());

        tracerLog()->emitBeginTransaction(false, error);
        m_transactionMutex.unlock();
        return false;
    }

    bool result = m_database.transaction();
    QString error = result?"":tr("Unable to begin transaction");

    tracerLog()->emitBeginTransaction(result, error);

    m_transactionIsOpen = result;
    if (!result)
        m_transactionMutex.unlock();
    return result;
}

bool IqOrmSqlDataSource::commitTransaction()
{
    Q_ASSERT(m_database.isValid());

    if (!m_transactionIsOpen) {
        QString error = tr("Transaction NOT open.");
        tracerLog()->emitCommitTransaction(false, error);
        return false;
    }

    bool result = m_database.commit();

    tracerLog()->emitCommitTransaction(result, m_database.lastError().text());

    m_transactionIsOpen = false;
    m_transactionMutex.unlock();
    return result;
}

bool IqOrmSqlDataSource::rollbackTransaction()
{
    Q_ASSERT(m_database.isValid());

    if (!m_transactionIsOpen) {
        QString error = tr("Transaction NOT open.");
        tracerLog()->emitRollbackTransaction(false, error);
        return false;
    }

    bool result = m_database.rollback();

    tracerLog()->emitRollbackTransaction(result, m_database.lastError().text());

    m_transactionIsOpen = false;
    m_transactionMutex.unlock();
    return result;
}


void IqOrmSqlDataSource::setDatabase(QSqlDatabase &database)
{
    m_database = database;

    QString dbType = m_database.driverName();
    if (dbType == QStringLiteral("QMYSQL"))
        m_databaseType = MySQL;
    else if (dbType == QStringLiteral("QSQLITE")
             || dbType == QStringLiteral("QSQLITE2"))
        m_databaseType = SQLite;
    else if (dbType == QStringLiteral("QPSQL"))
        m_databaseType = PostgreSQL;
    else if (dbType == QStringLiteral("QOCI"))
        m_databaseType = Oracle;
    else if (dbType == QStringLiteral("QODBC"))
        //Считаем, что это MSSQL
        m_databaseType = MSSQL;
    else
        m_databaseType = Unknown;

    m_sqlDriver = m_database.driver();
    m_escapedIdFieldName = m_sqlDriver->escapeIdentifier("id", QSqlDriver::FieldName);
}

bool IqOrmSqlDataSource::openDB() const
{
    if (!m_database.isValid()) {
        qWarning() << "Database not valid. Use IQORMSQLDataSource::setDatabase() first.";
        return false;
    }

    if (!m_database.isOpen() && !m_database.open()) {
        qWarning() << QString("Database open failed. SQL Error: \"%0\"").arg(m_database.lastError().text());
        return false;
    }

    return true;
}
IqOrmSqlOperationTracerLog *IqOrmSqlDataSource::tracerLog() const
{
    return m_tracerLog;
}

IqOrmSqlDataSource::DatabaseType IqOrmSqlDataSource::databaseType() const
{
    return m_databaseType;
}

QString IqOrmSqlDataSource::escapedIdFieldName() const
{
    return m_escapedIdFieldName;
}

QString IqOrmSqlDataSource::escapedFieldName(const QString &fieldName) const
{
    Q_CHECK_PTR(m_sqlDriver);
    return m_sqlDriver->escapeIdentifier(fieldName, QSqlDriver::FieldName);
}

QString IqOrmSqlDataSource::escapedTableName(const QString &tableName) const
{
    Q_CHECK_PTR(m_sqlDriver);
    return m_sqlDriver->escapeIdentifier(tableName, QSqlDriver::TableName);
}

QSqlQuery IqOrmSqlDataSource::execQuery(const QString &prepareString,
                                        const QList<QVariant> bindValues,
                                        bool forwardOnly,
                                        bool *ok,
                                        QString *errorText) const
{
    QTime elapsedTime;
    elapsedTime.start();
    if (ok)
        *ok = false;

    //Готовим запрос
    QSqlQuery query (m_database);
    query.setForwardOnly(forwardOnly);

    if (!prepareQuery(query, prepareString, errorText))
        return query;

    if (!execPreparedQuery(query, bindValues, errorText))
        return query;

    if (ok)
        *ok = true;

    return query;
}

bool IqOrmSqlDataSource::prepareQuery(QSqlQuery &query,
                                      const QString &prepareString,
                                      QString *errorText) const
{
    Q_ASSERT(m_database.isValid());

    QTime elapsedTime;
    elapsedTime.start();

    if (!openDB()) {
        QString error = tr("Can not open database \"%0\" on host \"%1:%2\" as \"%3\" with password \"*********\".")
                .arg(m_database.databaseName())
                .arg(m_database.hostName())
                .arg(m_database.port())
                .arg(m_database.userName());
        if (errorText)
            *errorText = error;

        tracerLog()->emitPrepareQuery(prepareString, elapsedTime.elapsed(), false, error);
        return false;
    }

    //Если не удалось подготовить запрос, то выходим
    if (!query.prepare(prepareString)) {
        QString error = tr("Can not prepare SQL query \"%0\". \nSQL Engine return error: \"%1\".")
                .arg(query.executedQuery())
                .arg(query.lastError().text());
        if (errorText)
            *errorText = error;

        tracerLog()->emitPrepareQuery(prepareString, elapsedTime.elapsed(), false, error);
        return false;
    }

    tracerLog()->emitPrepareQuery(prepareString, elapsedTime.elapsed(), true, "");
    return true;
}

bool IqOrmSqlDataSource::execPreparedQuery(QSqlQuery &query,
                                           const QList<QVariant> bindValues,
                                           QString *errorText) const
{
    Q_ASSERT(m_database.isValid());

    QTime elapsedTime;
    elapsedTime.start();

    if (!openDB()) {
        QString error = tr("Can not open database \"%0\" on host \"%1:%2\" as \"%3\" with password \"*********\".")
                .arg(m_database.databaseName())
                .arg(m_database.hostName())
                .arg(m_database.port())
                .arg(m_database.userName());
        if (errorText)
            *errorText = error;

        tracerLog()->emitExecPreparedQuery(bindValues, elapsedTime.elapsed(), false, error);
        return false;
    }

    //Биндим параметры
    foreach (QVariant value, bindValues) {
        query.addBindValue(value);
    }


    //Если не удалось выполнить запрос, то выходим
    if (!query.exec()) {
        QString error = tr("Can not exec SQL query \"%0\". \nSQL Engine return error: \"%1\".")
                .arg(query.executedQuery())
                .arg(query.lastError().text());
        if (errorText)
            *errorText = error;

        tracerLog()->emitExecPreparedQuery(bindValues, elapsedTime.elapsed(), false, error);
        return false;
    }

    tracerLog()->emitExecPreparedQuery(bindValues, elapsedTime.elapsed(), true, "");
    return true;
}

QString IqOrmSqlDataSource::escapedPropertyName(const QString &propertyName) const
{
    return escapedFieldName(propertyName);
}


QSqlQuery IqOrmSqlDataSource::execQuery(const QString &prepareString, const QList<QVariant> bindValues, bool *ok, QString *errorText) const
{
    return execQuery(prepareString, bindValues, false, ok, errorText);
}
