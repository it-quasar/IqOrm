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

    virtual IqOrmObject * createChildObject() Q_DECL_OVERRIDE;

    virtual const IqOrmMetaModel * childsOrmModel() const Q_DECL_OVERRIDE;

    virtual T *get(int row) const Q_DECL_OVERRIDE;

    virtual T *first() const Q_DECL_OVERRIDE;

    virtual T *last() const Q_DECL_OVERRIDE;

    virtual T *take(int row) Q_DECL_OVERRIDE;

    virtual T *take(IqOrmObject *object) Q_DECL_OVERRIDE;

    static bool truncate(QString *error);

    static bool truncate(IqOrmAbstractDataSource *dataSource,
                         QString *error);
    QList<T *> toList() const;

private:
    static const IqOrmMetaModel *staticChidsOrmModel();
};









template <class T>
IqOrmModel<T>::IqOrmModel(QObject *parent) :
    IqOrmBaseModel(parent)
{
    foreach (const IqOrmPropertyDescription *propertyDescription, childsOrmModel()->propertyDescriptions()) {
        addPropertySiagnalIndex(propertyDescription->propertyName(), propertyDescription->targetStaticMetaPropery().notifySignalIndex());
    }
}

template <class T>
IqOrmModel<T>::~IqOrmModel()
{
}

template <class T>
IqOrmObject * IqOrmModel<T>::createChildObject()
{
    //Не передавать новому объекту в конструктор lastDataSource(), т.к. датосурс кастится к QObject
    //и принимается за родителя!!!
    T* newObject = new T();
    newObject->setDataSource(lastDataSource());
    newObject->setParent(this);
    return newObject;
}

template <class T>
const IqOrmMetaModel* IqOrmModel<T>::childsOrmModel() const
{
    return staticChidsOrmModel();
}


template <class T>
T * IqOrmModel<T>::get(int row) const
{
    return qobject_cast<T *>(IqOrmBaseModel::get(row));
}

template <class T>
T * IqOrmModel<T>::first() const
{
    if (rowCount() == 0)
        return Q_NULLPTR;
    return get(0);
}

template <class T>
T * IqOrmModel<T>::last() const
{
    if (rowCount() == 0)
        return Q_NULLPTR;
    return get(rowCount() - 1);
}

template <class T>
T *IqOrmModel<T>::take(int row)
{
    return dynamic_cast<T *>(IqOrmBaseModel::take(row));
}

template <class T>
T *IqOrmModel<T>::take(IqOrmObject *object)
{
    return dynamic_cast<T *>(IqOrmBaseModel::take(object));
}

template <class T>
bool IqOrmModel<T>::truncate(QString *error)
{
    return processTruncate(staticChidsOrmModel(), IqOrmCore::dataSource(), error);
}

template <class T>
bool IqOrmModel<T>::truncate(IqOrmAbstractDataSource *dataSource, QString *error)
{
    return processTruncate(staticChidsOrmModel(), dataSource, error);
}

template <class T>
QList<T *> IqOrmModel<T>::toList() const
{
    QList<T *> result;
    for (int i = 0; i < rowCount(); ++i) {
        result << get(i);
    }

    return result;
}

template <class T>
const IqOrmMetaModel *IqOrmModel<T>::staticChidsOrmModel()
{
    return T::staticOrmMetaModel();
}

#endif // IQORMMODEL_H
