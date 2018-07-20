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

#ifndef IQORMMODEL_H
#define IQORMMODEL_H

#include "iqormbasemodel.h"
#include "iqorm_global.h"
#include "iqormcore.h"

template <class T>
class IqOrmModel : public IqOrmBaseModel
{
public:
    explicit IqOrmModel(QObject *parent = Q_NULLPTR);

    virtual ~IqOrmModel();

    virtual const IqOrmMetaModel * childsOrmMetaModel() const Q_DECL_OVERRIDE;

    virtual T *get(int row) Q_DECL_OVERRIDE;

    virtual T *first() Q_DECL_OVERRIDE;

    virtual T *last() Q_DECL_OVERRIDE;

    virtual T *take(int row) Q_DECL_OVERRIDE;

    virtual T *take(IqOrmObject *object) Q_DECL_OVERRIDE;

    virtual T *takeFirst() Q_DECL_OVERRIDE;

    static bool truncate(QString *error = Q_NULLPTR);

    static bool truncate(IqOrmTransactionControl transaction,
                         QString *error = Q_NULLPTR);

    QList<T *> takeAll();

    QList<T *> toList();

protected:
    virtual IqOrmObject * createChildObject() const Q_DECL_OVERRIDE;

private:
    static const IqOrmMetaModel *staticChidsOrmModel();
};

#include "iqormmodel_impl.h"

#endif // IQORMMODEL_H
