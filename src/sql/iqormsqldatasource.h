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

#ifndef IQORMSQLDATASOURCE_H
#define IQORMSQLDATASOURCE_H

#include "iqormabstractdatasource.h"
#include "iqormsqloperationtracerlog.h"
#include "iqorm_global.h"
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QHash>
#include <QVariant>
#include <QMutex>
#include "iqormsqlobjectdatasource.h"
#include "iqormsqlmodeldatasource.h"

class IqOrmObject;
class IqOrmMetaModel;

class IQORMSHARED_EXPORT IqOrmSqlDataSource : public IqOrmAbstractDataSource
{
    Q_OBJECT
    Q_ENUMS(Features)
public:
    enum DatabaseType {
        Unknown,
        PostgreSQL,
        MySQL,
        MSSQL,
        Oracle,
        SQLite
    };

    enum Features {
        ForeignKeys
    };

    explicit IqOrmSqlDataSource(QObject *parent = Q_NULLPTR);

    virtual IqOrmSqlObjectDataSource *objectDataSource() const Q_DECL_OVERRIDE;

    virtual IqOrmSqlModelDataSource *objectsModelDataSource() const Q_DECL_OVERRIDE;

    bool enableFeature(Features feature);

    bool disableFeature(Features feature);

    bool setFeature(Features feature, bool enabled = true);

    QSqlQuery execQuery(const QString &query,
                        bool *ok = Q_NULLPTR,
                        QString *errorText = Q_NULLPTR) const;

    QSqlQuery execQuery(const QString &prepareString,
                        const QList<QVariant> bindValues,
                        bool *ok = Q_NULLPTR,
                        QString *errorText = Q_NULLPTR) const;

    QSqlQuery execQuery(const QString &prepareString,
                        const QList<QVariant> bindValues,
                        bool forwardOnly,
                        bool *ok = Q_NULLPTR,
                        QString *errorText = Q_NULLPTR) const;

    QSqlQuery prepareQuery(const QString &prepareString,
                           bool *ok = Q_NULLPTR,
                           QString *errorText = Q_NULLPTR) const;

    bool execPreparedQuery(QSqlQuery &query,
                           const QList<QVariant> bindValues = QList<QVariant>(),
                           QString *errorText = Q_NULLPTR) const;

    QString escapedIdFieldName() const;

    QString escapedFieldName(const QString &fieldName) const;

    QString escapedTableName(const QString &tableName) const;

    QString escapedPropertyName(const QString &propertyName) const;

    static QVariant prepareValueToSql(const QVariant &value);

    static QVariant createValueFromSql(const QVariant &sqlValue,
                                       QVariant::Type valueType,
                                       bool *ok = Q_NULLPTR);

    static QString escapeString(const QString &string);

    static QStringList splitStringArray(const QString &arrayString,
                                        bool *ok = Q_NULLPTR);

    static QString joinStringArray(const QStringList &list);

protected:
    virtual bool openTransaction() Q_DECL_OVERRIDE;

    virtual bool commitTransaction() Q_DECL_OVERRIDE;

    virtual bool rollbackTransaction() Q_DECL_OVERRIDE;

public:
    inline QSqlDatabase database () const {return m_database;}
    void setDatabase(QSqlDatabase &database);

    DatabaseType databaseType() const;

    void setTracerLog(IqOrmSqlOperationTracerLog *tracerLog);
    IqOrmSqlOperationTracerLog *tracerLog() const;

private:
    friend class IqOrmSqlObjectDataSource;
    friend class IqOrmSqlModelDataSource;
    friend class IqOrmSqlAbstractPropertyDescriptionProcessor;

    bool openDB(QString *error = Q_NULLPTR) const;

    bool setForeignKeysFeature(bool enabled) const;

private:
    mutable QSqlDatabase m_database;
    DatabaseType m_databaseType;

    QString m_escapedIdFieldName;
    QSqlDriver *m_sqlDriver;

    IqOrmSqlObjectDataSource *m_objectDataSource;
    IqOrmSqlModelDataSource *m_objectsModelDataSource;
    bool m_transactionIsOpen;
    QMutex m_transactionMutex;

    IqOrmSqlOperationTracerLog *m_tracerLog;
    QHash<Features, bool> m_changedFeatures;
};

#endif // IQORMSQLDATASOURCE_H
