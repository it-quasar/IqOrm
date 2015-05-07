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

#ifndef IQORMDATASOURCEOPERATIONRESULT_H
#define IQORMDATASOURCEOPERATIONRESULT_H

#include <QObject>
#include <QMultiHash>
#include <QSharedData>
#include "iqormdatasourcechanges.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmDataSourceOperationResult
{
public:
    IqOrmDataSourceOperationResult();

    operator bool() const;

    bool result() const;

    QList<IqOrmDataSourceChanges> allChanges() const;

    IqOrmDataSourceChanges *changes(const IqOrmMetaModel *model,
                                    qint64 objectId);

public:
    IqOrmAbstractDataSource::Operation operation() const;
    void setOperation(const IqOrmAbstractDataSource::Operation &operation);

    qint64 objectId() const;
    void setObjectId(qint64 objectId);

    QString error() const;
    void setError(const QString &error);

    IqOrmAbstractDataSource *dataSource() const;
    void setDataSource(IqOrmAbstractDataSource *dataSource);


private:
    class IqOrmDataSourceOperationResultData: public QSharedData
    {
    public:
        qint64 objectId;
        IqOrmAbstractDataSource::Operation operation;
        QString error;
        IqOrmAbstractDataSource *dataSource;
        QMultiHash<const IqOrmMetaModel*, IqOrmDataSourceChanges *> changes;

        IqOrmDataSourceOperationResultData();
        ~IqOrmDataSourceOperationResultData();
    };
    QSharedDataPointer<IqOrmDataSourceOperationResultData> d;
};

#endif // IQORMDATASOURCEOPERATIONRESULT_H
