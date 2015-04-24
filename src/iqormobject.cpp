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

#include "iqormobject.h"
#include "iqormcore.h"
#include "iqormerror.h"
#include "iqormabstractdatasource.h"
#include "iqormpropertydescription.h"
#include "iqormabstractobjectdatasource.h"
#include "iqormmetamodel.h"
#include "iqormabstracttriggers.h"
#include "iqormobjectrawdata.h"
#include <QPointer>
#include <QDebug>

IqOrmObject::IqOrmObject(IqOrmAbstractDataSource *dataSource):
    m_objectId(-1),
    m_dataSource(dataSource),
    m_lastError(new IqOrmError(dynamic_cast<QObject *>(this))),
    m_isLoadedFromDataSource(false)
{    
}

IqOrmObject::~IqOrmObject()
{
}

const IqOrmMetaModel *IqOrmObject::ormMetaModel() const
{
    return Q_NULLPTR;
}

qint64 IqOrmObject::objectId() const
{
    return m_objectId;
}

bool IqOrmObject::isValid() const
{
    return isLoadedFromDataSource() && isSavedToDataSource();
}

IqOrmAbstractDataSource *IqOrmObject::dataSource() const
{
    return m_dataSource;
}

bool IqOrmObject::load(const qint64 id, bool autoTransaction)
{
    m_lastError->clearError();

    QString error;
    if (triggers() && !triggers()->preLoad(this, id, &error)) {
        m_lastError->setText(QObject::tr("Load aborted by preLoad() trigger with error: \"%0\"")
                             .arg(error));
        return false;
    }

    if (id < 0) {
        m_lastError->setText(QObject::tr("ObjectId must be > -1"));
        return false;
    }

    if (!ormMetaModel()) {
        m_lastError->setText(QObject::tr("Not found valid object ORM model."));
        return false;
    }

    IqOrmAbstractDataSource *ds = usedDataSource();
    Q_CHECK_PTR(ds);

    IqOrmTransactionControl transactionControl;
    if (autoTransaction)
        transactionControl = ds->transaction();
    IqOrmDataSourceOperationResult result = ds->objectDataSource()->loadObjectById(this, id);

    if (result) {
        //Сохраним значения источника
        updateSourcePropertyValues();
    } else {
        //Сбросим значения источника
        clearSourcePropertyValues();
    }

    if (result) {
        if (triggers() && !triggers()->postLoad(this, id, result, &error)) {
        m_lastError->setText(QObject::tr("Load aborted by postLoad() trigger with error: \"%0\"")
                             .arg(error));
            return false;
        }

        if (autoTransaction)
            transactionControl.commit();
    } else {
        m_lastError->setText(result.error());
    }

    return result;
}

bool IqOrmObject::load(bool autoTransaction)
{
    return reload(autoTransaction);
}

bool IqOrmObject::reload(bool autoTransaction)
{
    m_lastError->clearError();

    if (objectId() == -1) {
        m_lastError->setText(QObject::tr("Can not reload object without objectId. Use load(qint64 objectId) first."));
        return false;
    }

    return load(objectId(), autoTransaction);
}

void IqOrmObject::setDataSource(IqOrmAbstractDataSource *dataSource)
{
    if (m_dataSource != dataSource) {
        m_dataSource = dataSource;

        emitDataSourceChanged();

        //Сбросим ид объекта, т.к. в другом источнике данных объект для устновки ид должен быть загружен
        setObjectId(-1);
    }
}

const IqOrmError *IqOrmObject::lastError() const
{
    return m_lastError;
}

IqOrmAbstractDataSource *IqOrmObject::usedDataSource() const
{
    if (m_dataSource)
        return m_dataSource;

    IqOrmAbstractDataSource *coreDataSource = IqOrmCore::dataSource();
    Q_CHECK_PTR (coreDataSource);
    Q_CHECK_PTR(coreDataSource->objectDataSource());

    return coreDataSource;
}

bool IqOrmObject::save(bool autoTransaction)
{
    m_lastError->clearError();

    bool isNewObject = !isSavedToDataSource();
    bool isLoadedObject = !isNewObject && isLoadedFromDataSource();
    Q_ASSERT(isNewObject != isLoadedObject);

    if (!isNewObject && !isLoadedObject) {
        m_lastError->setText(QObject::tr("Object exist in Data Source, but was not loaded from it. Can "
                                         "not save(). Use load() or reload() first."));
    }

    IqOrmAbstractTriggers::SaveType saveType = isNewObject?IqOrmAbstractTriggers::Persist:IqOrmAbstractTriggers::Update;

    QList<const IqOrmPropertyDescription *> propertiesToSave;
    switch (saveType) {
    case IqOrmAbstractTriggers::Persist:
        break;
    case IqOrmAbstractTriggers::Update:
        propertiesToSave = sourcePropertyChanges();
        break;
    }

    QString error;
    if (triggers() && !triggers()->preSave(this, saveType, propertiesToSave, &error)) {
            m_lastError->setText(QObject::tr("Save aborted by preSave() trigger with error: \"%0\"")
                                 .arg(error));
            return false;
    }

    if (!ormMetaModel()) {
        m_lastError->setText(QObject::tr("Not found valid object ORM model."));
        return false;
    }

    IqOrmAbstractDataSource *ds = usedDataSource();
    Q_CHECK_PTR (ds);

    IqOrmDataSourceOperationResult result;

    IqOrmTransactionControl transactionControl;
    if (autoTransaction)
        transactionControl = ds->transaction();

    switch (saveType) {
    case IqOrmAbstractTriggers::Persist: {
        //Создаем новый объект
        result = ds->objectDataSource()->insertObject(this);

        if (result)
            updateSourcePropertyValues();

        break;
    }
    case IqOrmAbstractTriggers::Update: {
        if (propertiesToSave.isEmpty()) {
            qDebug() << QObject::tr("Call save() for an object that has no changes.");
            return true;
        }

        //Обновляем существующий объект
        result = ds->objectDataSource()->updateObject(this, propertiesToSave);

        if (result)
            updateSourcePropertyValues();

        break;
    }
    }

    if (result) {
        if (triggers() && !triggers()->postSave(this, saveType, result, &error)) {
            m_lastError->setText(QObject::tr("Save aborted by postSave() trigger with error: \"%0\"")
                                 .arg(error));
            return false;
        }

        if (autoTransaction)
            transactionControl.commit();
    } else {
        m_lastError->setText(result.error());
    }

    return result;
}

void IqOrmObject::setObjectId(qint64 objectId)
{
    if (m_objectId != objectId) {
        bool oldIsValid = isValid();
        m_objectId = objectId;

        emitObjectIdChanged();

        if (isValid() != oldIsValid)
            emitIsValidChanged();

        setIsLoadedFromDataSource(objectId != -1);
    }
}

bool IqOrmObject::remove(bool autoTransaction)
{
    m_lastError->clearError();

    QString error;
    if (triggers() && !triggers()->preRemove(this, &error)) {
        m_lastError->setText(QObject::tr("Remove aborted by preRemove() trigger with error: \"%0\"")
                             .arg(error));
        return false;
    }

    if (!isSavedToDataSource()) {
        m_lastError->setText(QObject::tr("Unable to delete an object that has not been saved in Data Source. "
                                         "Use load() or save() first."));
        return false;
    }

    if (!sourcePropertyChanges().isEmpty()) {
        qDebug() << QObject::tr("Deleted object in which a change is made. Will reload() the object.");
        if (!reload())
            return false;
    }

    IqOrmAbstractDataSource *ds = usedDataSource();
    Q_CHECK_PTR (ds);

    IqOrmTransactionControl transactionControl;
    if (autoTransaction)
        transactionControl = ds->transaction();

    qint64 id = objectId();
    IqOrmDataSourceOperationResult result = ds->objectDataSource()->removeObject(this);

    if (result) {
        if (triggers() && !triggers()->postRemove(this, id, result, &error)) {
            m_lastError->setText(QObject::tr("Remove aborted by postRemove() trigger with error: \"%0\"")
                                 .arg(error));
            return false;
        }

        if (autoTransaction)
            transactionControl.commit();
    } else {
        m_lastError->setText(result.error());
    }

    return result;
}

void IqOrmObject::updateSourcePropertyValues()
{
    foreach (const IqOrmPropertyDescription *propDescription, ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propDescription);
        switch (propDescription->storedValue()) {
        case IqOrmPropertyDescription::SimpeVariant:
            m_sourcePropertyValues[propDescription] = propDescription->value(this);
            break;
        case IqOrmPropertyDescription::ObjectPointer: {
            const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
            m_sourcePropertyValues[propDescription] = oneObjectDescribingPropertyDescription->valueAsObjectId(this);
            break;
        }
        case IqOrmPropertyDescription::ObjectPointerList: {
            const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
            m_sourcePropertyValues[propDescription] = manyObjectDescribingPropertyDescription->valueAsObjectIds(this);
            break;
        }
        }
    }
}

void IqOrmObject::clearSourcePropertyValues()
{
    m_sourcePropertyValues.clear();
}

QList<const IqOrmPropertyDescription *> IqOrmObject::sourcePropertyChanges() const
{
    QList<const IqOrmPropertyDescription *> result;
    foreach (const IqOrmPropertyDescription *propDescription, ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propDescription);
        switch (propDescription->storedValue()) {
        case IqOrmPropertyDescription::SimpeVariant:
            if (m_sourcePropertyValues[propDescription] != propDescription->value(this))
                result << propDescription;
            break;
        case IqOrmPropertyDescription::ObjectPointer: {
            const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
            QVariant currentObjectId = oneObjectDescribingPropertyDescription->valueAsObjectId(this);
            QVariant sourceObjectId = m_sourcePropertyValues[propDescription];
            if(sourceObjectId != currentObjectId)
                result << propDescription;
            break;
        }
        case IqOrmPropertyDescription::ObjectPointerList: {
            const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
            QVariantList curreontObjectIds = manyObjectDescribingPropertyDescription->valueAsObjectIds(this);
            QVariantList sourceObjectIds = m_sourcePropertyValues[propDescription].toList();
            if(sourceObjectIds != curreontObjectIds)
                result << propDescription;
            break;
        }
        }
    }

    return result;
}

QVariant IqOrmObject::sourcePropertyValue(const IqOrmPropertyDescription *propertyDescription) const
{
    return m_sourcePropertyValues[const_cast<IqOrmPropertyDescription *>(propertyDescription)];
}

IqOrmAbstractTriggers *IqOrmObject::triggers() const
{
    Q_CHECK_PTR(ormMetaModel());

    if (ormMetaModel()->triggers())
        return ormMetaModel()->triggers();

    return IqOrmCore::defaultTriggers();
}

bool IqOrmObject::isLoadedFromDataSource() const
{
    return m_isLoadedFromDataSource;
}

void IqOrmObject::setIsLoadedFromDataSource(bool isLoadedFromDataSource)
{
    m_isLoadedFromDataSource = isLoadedFromDataSource;
}

bool IqOrmObject::isSavedToDataSource() const
{
    return objectId() != -1;
}

void IqOrmObject::setValues(const IqOrmObjectRawData &rawData)
{
    foreach (const IqOrmPropertyDescription *propDescription, ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propDescription);
        Q_ASSERT(rawData.values.contains(propDescription));

        //Устанавливаем значение
        switch (propDescription->storedValue()) {
        case IqOrmPropertyDescription::Direct: {
            propDescription->setValue(this, rawData.values[propDescription]);
            break;
        }
        case IqOrmPropertyDescription::ObjectPointer: {
            const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
            bool setResult = oneObjectDescribingPropertyDescription->setValueFromObjectId(this, rawData.values[propDescription].toLongLong());
            Q_UNUSED(setResult);
            Q_ASSERT(setResult);
            break;
        }
        case IqOrmPropertyDescription::ObjectPointerList: {
            const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
            bool setResult = manyObjectDescribingPropertyDescription->setValueFromObjectIds(this, rawData.values[propDescription].toList());
            Q_UNUSED(setResult);
            Q_ASSERT(setResult);
            break;
        }
        }
    }

    setObjectId(rawData.objectId);
}
