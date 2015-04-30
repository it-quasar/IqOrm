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

#ifndef IQORMSQLMODELDATASOURCE_H
#define IQORMSQLMODELDATASOURCE_H

#include "iqormabstractmodeldatasource.h"
#include "iqorm_global.h"
#include "iqormfilter.h"
#include <QSqlDriver>

class IqOrmAbstractFilter;
class IqOrmSqlDataSource;
class IqOrmDirectPropertyDescription;
class IqOrmBaseManyToOnePropertyDescription;

class IQORMSHARED_EXPORT IqOrmSqlModelDataSource : public IqOrmAbstractModelDataSource
{
    Q_OBJECT
public:
    explicit IqOrmSqlModelDataSource(IqOrmSqlDataSource *sqlDataSource = Q_NULLPTR);

    virtual IqOrmDataSourceOperationResult loadModel(IqOrmBaseModel *model,
                                                     qint64 limit = -1,
                                                     qint64 offset = 0,
                                                     OrderBy orderBy = Asc) Q_DECL_OVERRIDE;

    virtual IqOrmDataSourceOperationResult truncateModel(const IqOrmMetaModel *ormModel) Q_DECL_OVERRIDE;

private:
    IqOrmSqlDataSource* m_sqlDataSource;

    QString filterString(const IqOrmMetaModel *ormModel,
                         const IqOrmAbstractFilter *filter,
                         QVariantList *bindValues,
                         bool *ok,
                         QString *errorString) const;

    QString simpleFilterString(const IqOrmMetaModel *ormModel,
                               const IqOrmFilter *filter,
                               QVariantList *bindValues,
                               bool *ok,
                               QString *errorString) const;

    QString filterStringForDirectProperty(const IqOrmMetaModel *ormModel,
                                         const IqOrmFilter *filter,
                                         const IqOrmDirectPropertyDescription *propertyDesctiption,
                                         QVariantList *bindValues,
                                          bool *ok,
                                          QString *errorString) const;

    QString filterStringForManyToOneProperty(const IqOrmMetaModel *ormModel,
                                             const IqOrmFilter *filter,
                                             const IqOrmBaseManyToOnePropertyDescription *propertyDesctiption,
                                             QVariantList *bindValues,
                                             bool *ok,
                                             QString *errorString) const;

    QString columnCondition(const QString &columnName,
                            const QString &propertyName,
                            IqOrmFilter::Condition condition,
                            bool *ok,
                            QString *errorString) const;
};

#endif // IQORMSQLMODELDATASOURCE_H
