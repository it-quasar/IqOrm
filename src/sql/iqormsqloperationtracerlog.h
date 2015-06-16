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

#ifndef IQORMSQLOPERATIONTRACERLOG_H
#define IQORMSQLOPERATIONTRACERLOG_H

#include <QObject>
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlOperationTracerLog : public QObject
{
    Q_OBJECT
public:
    explicit IqOrmSqlOperationTracerLog(QObject *parent = Q_NULLPTR);

signals:
    void beginTransaction(bool ok, const QString &error);
    void commitTransaction(bool ok, const QString &error);
    void rollbackTransaction(bool ok, const QString &error);
    void prepareQuery(const QString &query, qint32 elapsed, bool ok, const QString &error);
    void execQuery(const QString &query, qint32 elapsed,  bool ok, const QString &error);
    void execPreparedQuery(const QVariantList &bindValues, qint32 elapsed, bool ok, const QString &error);

private:
    friend class IqOrmSqlDataSource;

    void emitBeginTransaction(bool ok, const QString &error);
    void emitCommitTransaction(bool ok, const QString &error);
    void emitRollbackTransaction(bool ok, const QString &error);
    void emitPrepareQuery(const QString &query, qint32 elapsed,  bool ok, const QString &error);
    void emitExecQuery(const QString &query, qint32 elapsed,  bool ok, const QString &error);
    void emitExecPreparedQuery(const QVariantList &bindValues, qint32 elapsed, bool ok, const QString &error);

};

#endif // IQORMSQLOPERATIONTRACERLOG_H
