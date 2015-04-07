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

#ifndef IQORMSQLOBJECTSMODELDATASOURCE_H
#define IQORMSQLOBJECTSMODELDATASOURCE_H

#include "iqormabstractobjectsmodeldatasource.h"
#include "iqorm_global.h"
#include <QSqlDriver>

class IqOrmAbstractFilter;
class IqOrmSqlDataSource;

class IQORMSHARED_EXPORT IqOrmSqlObjectsModelDataSource : public IqOrmAbstractObjectsModelDataSource
{
    Q_OBJECT
public:
    explicit IqOrmSqlObjectsModelDataSource(IqOrmSqlDataSource *sqlDataSource = Q_NULLPTR);

    virtual IqOrmDataSourceOperationResult loadModel(IqOrmBaseModel *model,
                                                     qint64 limit = -1,
                                                     qint64 offset = 0,
                                                     OrderBy orderBy = Asc) Q_DECL_OVERRIDE;

    virtual IqOrmDataSourceOperationResult truncateModel(const IqOrmMetaModel *ormModel) Q_DECL_OVERRIDE;

//    virtual QList<qint64> findObjects(const IqOrmModel *ormModel,
//                                      const IqOrmAbstractFilter *filter,
//                                      qint64 limit = -1,
//                                      qint64 offset = 0,
//                                      bool *ok = Q_NULLPTR,
//                                      QString *errorMessage = Q_NULLPTR) const Q_DECL_OVERRIDE;

private:
    IqOrmSqlDataSource* m_sqlDataSource;

    QString filterString(const IqOrmMetaModel *ormModel,
                         const IqOrmAbstractFilter *filter,
                         QVariantList *bindValues) const;

//    QStringList referencingTableNames(const IqOrmModel *ormModel,
//                                      const IqOrmAbstractFilter *filter) const;
};

#endif // IQORMSQLOBJECTSMODELDATASOURCE_H
