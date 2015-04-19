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

#ifndef IQORMABSTRACTMODELDATASOURCE_H
#define IQORMABSTRACTMODELDATASOURCE_H

#include <QObject>
#include "iqorm_global.h"

class IqOrmAbstractDataSource;
class IqOrmBaseModel;
class IqOrmMetaModel;
class IqOrmDataSourceOperationResult;

class IQORMSHARED_EXPORT IqOrmAbstractModelDataSource : public QObject
{
    Q_OBJECT
public:
    enum OrderBy
    {
        Asc,
        Desc
    };

    explicit IqOrmAbstractModelDataSource(IqOrmAbstractDataSource *parent = Q_NULLPTR);

    virtual ~IqOrmAbstractModelDataSource();

    virtual IqOrmDataSourceOperationResult loadModel(IqOrmBaseModel *model,
                                                     qint64 limit = -1,
                                                     qint64 offset = 0,
                                                     OrderBy orderBy = Asc) = 0;

    virtual IqOrmDataSourceOperationResult truncateModel(const IqOrmMetaModel *ormModel) = 0;
};

#endif // IQORMABSTRACTMODELDATASOURCE_H
