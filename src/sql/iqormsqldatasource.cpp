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
#include "iqormsqlobjectdatasource.h"
#include "iqormsqlmodeldatasource.h"
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
    m_objectsModelDataSource(new IqOrmSqlModelDataSource(this)),
    m_transactionIsOpen(false),
    m_tracerLog(new IqOrmSqlOperationTracerLog(this))
{
}

QVariant IqOrmSqlDataSource::prepareValueToSql(const QVariant &value)
{
    switch (static_cast<QMetaType::Type>(value.type())) {
    case QMetaType::UChar:
    case QMetaType::Char:
        return value.toInt();
        break;
    case QMetaType::QStringList:
        return joinStringArray(value.toStringList());
        break;
    default:
        break;
    }

    return value;
}

QVariant IqOrmSqlDataSource::createValueFromSql(const QVariant &sqlValue,
                                                QVariant::Type valueType,
                                                bool *ok)
{
    QVariant result;

    switch (valueType) {
    case QVariant::StringList: {
        //Если это список строк
        bool splitOk;
        result = IqOrmSqlDataSource::splitStringArray(sqlValue.toString(), &splitOk);

        if (!splitOk) {
            if (ok)
                *ok = false;
            return result;
        }

        break;
    }
    default:
        result = sqlValue;
        break;
    }

    if (ok)
        *ok = true;

    return result;
}

QString IqOrmSqlDataSource::escapeString(const QString &string)
{
    if (string.contains(',')
            || string.contains('\\')
            || string.contains('\'')) {
        QString quotedString = string;
        quotedString.replace('\\', "\\\\");
        quotedString.replace('\'', "\\'");
        quotedString.prepend('\'');
        quotedString.append('\'');
        return quotedString;
    }

    return string;
}

QStringList IqOrmSqlDataSource::splitStringArray(const QString &arrayString, bool *ok)
{
    if (arrayString.isEmpty()) {
        if (ok)
            *ok = true;
        return QStringList();
    }

    if (!arrayString.startsWith("{")
            || !arrayString.endsWith("}")) {
        if (ok)
            *ok = false;
        return QStringList();
    }

    QStringList result;

    QString string;
    bool quoteStart = false;
    for (int i = 1; i < arrayString.length() - 1; ++i) {
        QChar c = arrayString.at(i);

        if (c == ',') {
            if (quoteStart)
                string.append(c);
            else {
                result.append(string);
                string.clear();
            }
        } else if (c == '\\') {
            ++i;
            QChar nextC = arrayString.at(i);
            string.append(nextC);
        } else if (c == '\'')
            quoteStart = !quoteStart;
        else
            string.append(c);
    }

    result.append(string);

    if (ok)
        *ok = true;

    return result;
}

QString IqOrmSqlDataSource::joinStringArray(const QStringList &list)
{
    if (list.isEmpty())
        return "";

    QStringList escapedStringList;
    foreach (const QString &string, list) {
        escapedStringList.append(escapeString(string));
    }

    QString result = escapedStringList.join(',');
    result.prepend('{');
    result.append('}');
    return result;
}

IqOrmSqlObjectDataSource * IqOrmSqlDataSource::objectDataSource() const
{
    return m_objectDataSource;
}

IqOrmSqlModelDataSource * IqOrmSqlDataSource::objectsModelDataSource() const
{
    return m_objectsModelDataSource;
}

bool IqOrmSqlDataSource::enableFeature(IqOrmSqlDataSource::Features feature)
{
    return setFeature(feature, true);
}

bool IqOrmSqlDataSource::disableFeature(IqOrmSqlDataSource::Features feature)
{
    return setFeature(feature, false);
}

bool IqOrmSqlDataSource::setFeature(IqOrmSqlDataSource::Features feature, bool enabled)
{
    Q_ASSERT_X(m_database.isValid(),
               Q_FUNC_INFO,
               "Set database first");

    switch (feature) {
    case ForeignKeys:
        return setForeignKeysFeature(enabled);
        break;
    }

#if defined(IQORM_DEBUG_MODE)
    int indexOfFeaturesEnumator = staticMetaObject.indexOfEnumerator("Features");
    Q_ASSERT(indexOfFeaturesEnumator != -1);
    QMetaEnum enumator = staticMetaObject.enumerator(indexOfFeaturesEnumator);
    qDebug("Future \"%s\" not support database with type \"%s\".",
           enumator.key(feature),
           m_database.driverName().toLocal8Bit().constData());
#endif

    return false;
}

QSqlQuery IqOrmSqlDataSource::execQuery(const QString &query, bool *ok, QString *errorText) const
{
    Q_ASSERT(m_database.isValid());

    QTime elapsedTime;
    elapsedTime.start();

    QString openDbError;
    if (!openDB(&openDbError)) {
        if (ok)
            *ok = false;
        if (errorText)
            *errorText = openDbError;

        tracerLog()->emitExecQuery(query, elapsedTime.elapsed(), false, openDbError);
        return QSqlQuery(m_database);
    }

    //Готовим запрос
    QSqlQuery sqlQuery (m_database);

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

    QString error;
    if (!openDB(&error)) {
        tracerLog()->emitBeginTransaction(false, error);
        m_transactionMutex.unlock();
        return false;
    }

    bool result = m_database.transaction();
    error = result?"":tr("Unable to begin transaction");

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

    QString error;
    if (!openDB(&error)) {
        tracerLog()->emitCommitTransaction(false, error);
        m_transactionIsOpen = false;
        m_transactionMutex.unlock();
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

    QString error;
    if (!openDB(&error)) {
        tracerLog()->emitRollbackTransaction(false, error);
        m_transactionIsOpen = false;
        m_transactionMutex.unlock();
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

    switch (m_database.driver()->dbmsType()) {
    case QSqlDriver::MySqlServer:
        m_databaseType = MySQL;
        break;
    case QSqlDriver::SQLite:
        m_databaseType = SQLite;
        break;
    case QSqlDriver::PostgreSQL:
        m_databaseType = PostgreSQL;
        break;
    case QSqlDriver::Oracle:
        m_databaseType = Oracle;
        break;
    case QSqlDriver::MSSqlServer:
        m_databaseType = MSSQL;
        break;
    default:
        m_databaseType = Unknown;
        break;
    }

    m_sqlDriver = m_database.driver();

    if (m_sqlDriver)
        m_escapedIdFieldName = m_sqlDriver->escapeIdentifier("id", QSqlDriver::FieldName);

    if (m_database.isValid()) {
        //Установим все расширения по новой
        QHashIterator<Features, bool> changedFeaturesI(m_changedFeatures);
        while(changedFeaturesI.hasNext()) {
            changedFeaturesI.next();
            setFeature(changedFeaturesI.key(), changedFeaturesI.value());
        }
    }
}

bool IqOrmSqlDataSource::openDB(QString *error) const
{
    bool result = true;
    QString errorText;

    if (!m_database.isValid()) {
        errorText = "Database not valid. Use IqOrmSqlDataSource::setDatabase() first.";
        result = false;
    } else if (!m_database.isOpen() && !m_database.open()) {
        errorText = QString("Database open failed. SQL Error: \"%0\"")
                .arg(m_database.lastError().text());
        result = false;
    }

    if (!result) {
        qWarning() << errorText;
        if (error)
            *error = tr("Can not open database \"%0\" on host \"%1:%2\" as \"%3\" with password \"*********\". Error: \"%4\".")
                .arg(m_database.databaseName())
                .arg(m_database.hostName())
                .arg(m_database.port())
                .arg(m_database.userName())
                .arg(errorText);
    } else {
        if (error)
            error->clear();
    }

    return result;
}

bool IqOrmSqlDataSource::setForeignKeysFeature(bool enabled) const
{
    bool result;
    switch (m_databaseType) {
    case SQLite: {
        QString query = QString("PRAGMA foreign_keys = %0;")
                .arg(enabled?"ON":"OFF");

        bool ok;
        execQuery(query, &ok);
        if (!ok)
            result = false;
        else {
            query = "PRAGMA foreign_keys;";
            QSqlQuery resultQuery =execQuery(query, &ok);
            if (!ok)
                result = false;
            else {

                if (!resultQuery.next()) {
#if defined(IQORM_DEBUG_MODE)
                    qWarning("\"PRAGMA foreign_keys\" not support with this SQLite version.");
#endif
                    result = false;
                } else {
                    if (enabled)
                        result = resultQuery.value(0).toInt() == 1;
                    else
                        result = resultQuery.value(0).toInt() != 1;
                }
            }
        }

        break;
    }
    default:
#if defined(IQORM_DEBUG_MODE)
        qDebug("Feature \"ForeingKeys\" not support database with type \"%s\".",
               m_database.driverName().toLocal8Bit().constData());
#endif
        return false;
        break;
    }

#if defined(IQORM_DEBUG_MODE)
    if (!result)
        qWarning("Error. Set \"ForeignKeys\" feature fail.");
#endif

    return result;
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
    bool prepareOk;
    QSqlQuery query = prepareQuery(prepareString, &prepareOk, errorText);
    if (!prepareOk)
        return query;

    query.setForwardOnly(forwardOnly);

    if (!execPreparedQuery(query, bindValues, errorText))
        return query;

    if (ok)
        *ok = true;

    return query;
}

QSqlQuery IqOrmSqlDataSource::prepareQuery(const QString &prepareString,
                                           bool *ok,
                                           QString *errorText) const
{
    Q_ASSERT(m_database.isValid());

    QTime elapsedTime;
    elapsedTime.start();

    QString openDbError;
    if (!openDB(&openDbError)) {
        if (ok)
            *ok = false;
        if (errorText)
            *errorText = openDbError;

        tracerLog()->emitPrepareQuery(prepareString, elapsedTime.elapsed(), false, openDbError);
        return QSqlQuery(m_database);
    }

    QSqlQuery query(m_database);
    //Если не удалось подготовить запрос, то выходим
    if (!query.prepare(prepareString)) {
        QString error = tr("Can not prepare SQL query \"%0\". \nSQL Engine return error: \"%1\".")
                .arg(query.executedQuery())
                .arg(query.lastError().text());
        if (ok)
            *ok = false;
        if (errorText)
            *errorText = error;

        tracerLog()->emitPrepareQuery(prepareString, elapsedTime.elapsed(), false, error);
        return query;
    }

    if (ok)
        *ok = true;
    if (errorText)
        errorText->clear();

    tracerLog()->emitPrepareQuery(prepareString, elapsedTime.elapsed(), true, "");
    return query;
}

bool IqOrmSqlDataSource::execPreparedQuery(QSqlQuery &query,
                                           const QList<QVariant> bindValues,
                                           QString *errorText) const
{
    Q_ASSERT(m_database.isValid());

    QTime elapsedTime;
    elapsedTime.start();

    QString openDbError;
    if (!openDB(&openDbError)) {
        tracerLog()->emitExecPreparedQuery(bindValues, elapsedTime.elapsed(), false, openDbError);
        if (errorText)
            *errorText = openDbError;
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

