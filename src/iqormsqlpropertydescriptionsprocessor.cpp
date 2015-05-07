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

#include "iqormsqlpropertydescriptionsprocessor.h"

#include "iqormpropertydescription.h"
#include "iqormsqljoinoperation.h"
#include "iqormdatasourceoperationresult.h"
#include "iqormobject.h"

#include "iqormsqldirectpropertydescriptionprocessor.h"
#include "iqormsqlonetoonepropertydescriptionprocessor.h"
#include "iqormsqlinversedonetoonepropertydescriptionprocessor.h"
#include "iqormsqlonetomanypropertydescriptionprocessor.h"
#include "iqormsqlmanytoonepropertydescriptionprocessor.h"
#include "iqormsqlmanytomanypropertydescriptionprocessor.h"
#include "iqormsqlinversedmanytomanypropertydescriptionprocessor.h"

IqOrmSqlPropertyDescriptionsProcessor::IqOrmSqlPropertyDescriptionsProcessor(QObject *parent) :
    QObject(parent),
    m_sqlDataSource(Q_NULLPTR)
{
}

QStringList IqOrmSqlPropertyDescriptionsProcessor::selectFieldNames(const IqOrmMetaModel *ormModel) const
{
    Q_CHECK_PTR(ormModel);
    QStringList result;
    foreach (const IqOrmPropertyDescription *propertyDescription, ormModel->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        QString fieldName = processor(propertyDescription, Q_NULLPTR, ormModel)->selectFieldName();
        if (!fieldName.isEmpty())
            result << fieldName;
    }

    return result;
}

QStringList IqOrmSqlPropertyDescriptionsProcessor::insertFieldNames(const IqOrmMetaModel *ormModel) const
{
    Q_CHECK_PTR(ormModel);
    QStringList result;
    foreach (const IqOrmPropertyDescription *propertyDescription, ormModel->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        QString fieldName = processor(propertyDescription, Q_NULLPTR, ormModel)->insertFieldName();
        if (!fieldName.isEmpty())
            result << fieldName;
    }

    return result;
}

QStringList IqOrmSqlPropertyDescriptionsProcessor::updateFieldNames(const IqOrmMetaModel *ormModel,
                                                                    const QSet<const IqOrmPropertyDescription *> &propertyDescriptions) const
{
    Q_CHECK_PTR(ormModel);
    QStringList result;
    foreach (const IqOrmPropertyDescription *propertyDescription, propertyDescriptions) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        QString fieldName = processor(propertyDescription, Q_NULLPTR, ormModel)->updateFieldName();
        if (!fieldName.isEmpty())
            result << fieldName;
    }

    return result;
}

QVariantList IqOrmSqlPropertyDescriptionsProcessor::insertValues(const IqOrmObject *object) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    QVariantList result;
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        QVariant value = processor(propertyDescription, object)->insertValue();
        if (value.isValid())
            result << value;
    }

    return result;
}

QVariantList IqOrmSqlPropertyDescriptionsProcessor::updateValues(const IqOrmObject *object,
                                                                 const QSet<const IqOrmPropertyDescription *> &propertyDescriptions) const
{
    Q_CHECK_PTR(object);
    QVariantList result;
    foreach (const IqOrmPropertyDescription *propertyDescription, propertyDescriptions) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        QVariant value = processor(propertyDescription, object)->updateValue();
        if (value.isValid())
            result << value;
    }

    return result;
}

QList<IqOrmSqlJoinOperation> IqOrmSqlPropertyDescriptionsProcessor::selectJoinOperations(const IqOrmMetaModel *ormModel) const
{
    Q_CHECK_PTR(ormModel);
    QList<IqOrmSqlJoinOperation> result;
    foreach (const IqOrmPropertyDescription *propertyDescription, ormModel->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        result << processor(propertyDescription, Q_NULLPTR, ormModel)->selectJoinOperations();
    }

    return result;
}

bool IqOrmSqlPropertyDescriptionsProcessor::selectAllowed(const IqOrmObject *object,
                                                          IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if(!processor(propertyDescription, object)->selectAllowed(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::insertAllowed(const IqOrmObject *object,
                                                          IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->insertAllowed(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::updateAllowed(const IqOrmObject *object,
                                                          const QSet<const IqOrmPropertyDescription *> &propertyDescriptions,
                                                          IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    foreach (const IqOrmPropertyDescription *propertyDescription, propertyDescriptions) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->updateAllowed(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::removeAllowed(const IqOrmObject *object, IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->removeAllowed(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::preSelect(const IqOrmObject *object,
                                                      IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if(!processor(propertyDescription, object)->preSelect(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::preInsert(const IqOrmObject *object,
                                                      IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->preInsert(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::preUpdate(const IqOrmObject *object,
                                                      const QSet<const IqOrmPropertyDescription *> &propertyDescriptions,
                                                      IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    foreach (const IqOrmPropertyDescription *propertyDescription, propertyDescriptions) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->preUpdate(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::preRemove(const IqOrmObject *object, IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->preRemove(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::postSelect(const IqOrmObject *object,
                                                       IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if(!processor(propertyDescription, object)->postSelect(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::postInsert(const IqOrmObject *object,
                                                       IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->postInsert(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::postUpdate(const IqOrmObject *object,
                                                       const QSet<const IqOrmPropertyDescription *> &propertyDescriptions,
                                                       IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    foreach (const IqOrmPropertyDescription *propertyDescription, propertyDescriptions) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->postUpdate(result))
            return false;
    }

    return true;
}

bool IqOrmSqlPropertyDescriptionsProcessor::postRemove(const IqOrmObject *object, IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(object);
    Q_CHECK_PTR(object->ormMetaModel());
    foreach (const IqOrmPropertyDescription *propertyDescription, object->ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propertyDescription);
        if (propertyDescription->readOnly())
            continue;
        if(!processor(propertyDescription, object)->postRemove(result))
            return false;
    }

    return true;
}

void IqOrmSqlPropertyDescriptionsProcessor::setSqlDataSource(IqOrmSqlDataSource *sqlDataSource)
{
    if (m_sqlDataSource != sqlDataSource) {
        m_sqlDataSource = sqlDataSource;
    }
}

QSharedPointer<IqOrmSqlAbstractPropertyDescriptionProcessor> IqOrmSqlPropertyDescriptionsProcessor::processor(const IqOrmPropertyDescription *propertyDescription,
                                                                                                              const IqOrmObject *object,
                                                                                                              const IqOrmMetaModel *ormModel) const
{
    Q_CHECK_PTR(propertyDescription);
    QSharedPointer<IqOrmSqlAbstractPropertyDescriptionProcessor> result;
    switch (propertyDescription->mappedType()) {
    case IqOrmPropertyDescription::Direct:
        result = QSharedPointer<IqOrmSqlDirectPropertyDescriptionProcessor>::create();
        break;
    case IqOrmPropertyDescription::OneToOne:
        result = QSharedPointer<IqOrmSqlOneToOnePropertyDescriptionProcessor>::create();
        break;
    case IqOrmPropertyDescription::InversedOneToOne:
        result = QSharedPointer<IqOrmSqlInversedOneToOnePropertyDescriptionProcessor>::create();
        break;
    case IqOrmPropertyDescription::OneToMany:
        result = QSharedPointer<IqOrmSqlOneToManyPropertyDescriptionProcessor>::create();
        break;
    case IqOrmPropertyDescription::ManyToOne:
        result = QSharedPointer<IqOrmSqlManyToOnePropertyDescriptionProcessor>::create();
        break;
    case IqOrmPropertyDescription::ManyToMany:
        result = QSharedPointer<IqOrmSqlManyToManyPropertyDescriptionProcessor>::create();
        break;
    case IqOrmPropertyDescription::InversedManyToMany:
        result = QSharedPointer<IqOrmSqlInversedManyToManyPropertyDescriptionProcessor>::create();
        break;
    }

    result->setSqlDataSource(m_sqlDataSource);
    result->setPropertyDescription(propertyDescription);
    result->setOrmModel(ormModel);
    result->setObject(object);

    return result;
}
