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

#include "iqormsqloperationtracerlog.h"

IqOrmSqlOperationTracerLog::IqOrmSqlOperationTracerLog(QObject *parent) :
    QObject(parent)
{
}

void IqOrmSqlOperationTracerLog::emitBeginTransaction(bool ok, const QString &error)
{
    emit beginTransaction(ok, error.trimmed());
}

void IqOrmSqlOperationTracerLog::emitCommitTransaction(bool ok, const QString &error)
{
    emit commitTransaction(ok, error.trimmed());
}

void IqOrmSqlOperationTracerLog::emitRollbackTransaction(bool ok, const QString &error)
{
    emit rollbackTransaction(ok, error.trimmed());
}

void IqOrmSqlOperationTracerLog::emitPrepareQuery(const QString &query, qint32 elapsed, bool ok, const QString &error)
{
    emit prepareQuery(query.trimmed(), elapsed, ok, error.trimmed());
}

void IqOrmSqlOperationTracerLog::emitExecQuery(const QString &query, qint32 elapsed, bool ok, const QString &error)
{
    return execQuery(query.trimmed(), elapsed, ok, error.trimmed());
}

void IqOrmSqlOperationTracerLog::emitExecPreparedQuery(const QVariantList &bindValues, qint32 elapsed, bool ok, const QString &error)
{
    emit execPreparedQuery(bindValues, elapsed, ok, error.trimmed());
}
