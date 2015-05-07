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

#include "iqormmetamodel.h"
#include "iqormdirectpropertydescription.h"
#include "iqormpropertydescription.h"
#include "iqormobject.h"
#include "iqormonetoonepropertydescription.h"
#include "iqormmanytoonepropertydescription.h"
#include "iqormobjectfactory.h"
#include <QMetaProperty>
#include <QHash>
#include <QDebug>
#include "iqormabstracttriggers.h"

IqOrmMetaModel::IqOrmMetaModel(QObject *parent) :
    QObject(parent),
    m_tableName(""),
    m_targetStaticMetaObject(Q_NULLPTR),
    m_triggers(Q_NULLPTR),
    m_validationObject(Q_NULLPTR)
{
}

void IqOrmMetaModel::setTableName(const QString tableName)
{
    if (m_tableName != tableName)
    {
        m_tableName = tableName;

        emit tableNameChanged();
    }
}

bool IqOrmMetaModel::checkPropertyDescriptionIsValid(const IqOrmPropertyDescription *propertyDescription) const
{
    Q_CHECK_PTR(m_validationObject);

    //Попробуем найти свойство у данного объекта

    const QObject *lastObject = propertyDescription->lastPropertyObject(m_validationObject);

    if (!lastObject) {
        return false;
    }

    const QString lastPropName = propertyDescription->propertyName().split('.').last();

    int propertyIntex = lastObject->metaObject()->indexOfProperty(lastPropName.toLocal8Bit().constData());
    return propertyIntex != -1;
}

IqOrmAbstractTriggers *IqOrmMetaModel::triggers() const
{
    return m_triggers;
}

void IqOrmMetaModel::setTriggers(IqOrmAbstractTriggers *triggers)
{
    if (m_triggers != triggers) {
        if (m_triggers)
            m_triggers->deleteLater();
        if (triggers)
            triggers->setParent(this);
        m_triggers = triggers;
        emit triggersChanged();
    }
}


bool IqOrmMetaModel::addPropertyOrmDescription(IqOrmPropertyDescription *description)
{
    Q_CHECK_PTR (description);
    Q_ASSERT (!description->propertyName().isEmpty());

    //Если для данного свойства уже есть описание
    foreach (const IqOrmPropertyDescription *oldDescription, m_propertyDescriptions)
    {
        Q_CHECK_PTR (oldDescription);
        if (oldDescription->propertyName() == description->propertyName()) {
            IqOrmPropertyDescription *notConstOldDescription = const_cast<IqOrmPropertyDescription *>(oldDescription);
            notConstOldDescription->deleteLater();
            m_propertyDescriptions.remove(oldDescription);
            break;
        }
    }

    //Проверим, что свойство правильное
    Q_ASSERT_X(checkPropertyDescriptionIsValid(description),
               Q_FUNC_INFO,
               tr("IqOrm: Property \"%0\" no foud in %1. Check Q_PROPERTY.")
               .arg(description->propertyName())
               .arg(m_validationObject->metaObject()->className()).toLocal8Bit().constData());

    description->setParent(this);
    description->setTargetStaticMetaObject(targetStaticMetaObject());
    m_propertyDescriptions << description;

    return true;
}

void IqOrmMetaModel::setPropertyColumn(const QString &property,
                                   const QString &columnName)
{
    Q_ASSERT(!property.isEmpty());

    IqOrmDirectPropertyDescription *newDescription = new IqOrmDirectPropertyDescription(this);
    newDescription->setPropertyName(property);
    if (columnName.isEmpty()) {
        newDescription->setColumnName(property);
    } else {
        newDescription->setColumnName(columnName);
    }

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}

QSet<const IqOrmPropertyDescription *> IqOrmMetaModel::propertyDescriptions() const
{
    return m_propertyDescriptions;
}

//QQmlListProperty<IqOrmPropertyDescription> IqOrmMetaModel::qmlPropertyDescription()
//{
//    return QQmlListProperty<IqOrmPropertyDescription>(this,
//                                                      &m_propertyDescriptions,
//                                                      &qmlPropertyDescriptionAppend,
//                                                      &qmlPropertyDescriptionCount,
//                                                      &qmlPropertyDescriptionAt,
//                                                      &qmlPropertyDescriptionClear);
//}

//void IqOrmMetaModel::qmlPropertyDescriptionAppend(QQmlListProperty<IqOrmPropertyDescription> *property, IqOrmPropertyDescription *value)
//{
//    if (!property)
//        return;
//    if (!value)
//        return;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return;
//    model->addPropertyOrmDescription(value);
//}

//IqOrmPropertyDescription * IqOrmMetaModel::qmlPropertyDescriptionAt(QQmlListProperty<IqOrmPropertyDescription> *property, int index)
//{
//    if (!property)
//        return NULL;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return NULL;
//    if (index > 0 && index < model->m_propertyDescriptions.count())
//    {
//        return model->m_propertyDescriptions[index];
//    }
//    return NULL;
//}

//void IqOrmMetaModel::qmlPropertyDescriptionClear(QQmlListProperty<IqOrmPropertyDescription> *property)
//{
//    if (!property)
//        return;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return;
//    qDeleteAll(model->m_propertyDescriptions);
//    model->m_propertyDescriptions.clear();
//}

//int IqOrmMetaModel::qmlPropertyDescriptionCount(QQmlListProperty<IqOrmPropertyDescription> *property)
//{
//    if (!property)
//        return 0;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return 0;
//    return model->m_propertyDescriptions.count();
//}

const QMetaObject *IqOrmMetaModel::targetStaticMetaObject() const
{
    return m_targetStaticMetaObject;
}

void IqOrmMetaModel::setTargetStaticMetaObject(const QMetaObject *targetStaticMetaObject)
{
    if (m_targetStaticMetaObject != targetStaticMetaObject) {
        m_targetStaticMetaObject = targetStaticMetaObject;

        if (m_targetStaticMetaObject) {
            //Создадим новый объект валидации
            if (m_validationObject)
                delete m_validationObject;

            m_validationObject = dynamic_cast<QObject*>(IqOrmObjectFactory::create(targetStaticMetaObject->className(), this));
        }

        //Установим метообъект всем свойствам
        foreach (const IqOrmPropertyDescription *propDescription, m_propertyDescriptions) {
            Q_CHECK_PTR(propDescription);
            IqOrmPropertyDescription *notConstPropertyDescription = const_cast<IqOrmPropertyDescription *>(propDescription);
            notConstPropertyDescription->setTargetStaticMetaObject(targetStaticMetaObject);
        }
    }
}


const IqOrmPropertyDescription *IqOrmMetaModel::propertyDescription(const QString &propertyName) const
{
    foreach (const IqOrmPropertyDescription *description, m_propertyDescriptions)
    {
        Q_CHECK_PTR (description);
        if (description->propertyName() == propertyName)
            return description;
    }

    return NULL;
}

bool IqOrmMetaModel::isValid() const
{
    bool result = true;

    Q_CHECK_PTR(m_validationObject);

    if (tableName().isEmpty()) {
        qWarning() << tr("IqOrm: During the verification of the ORM model revealed that not set tableName.");

        result = false;
    }

    foreach (const IqOrmPropertyDescription *propDescription, m_propertyDescriptions)
    {
        Q_CHECK_PTR(propDescription);
        if (propDescription->propertyName().isEmpty())
        {
            qWarning() << tr("IqOrm: During the verification of the ORM model revealed that for "
                             "the description of the property name is not specified properties.");
            result = false;
        }
        else if (!checkPropertyDescriptionIsValid(propDescription))
        {
            qWarning() << tr("IqOrm: During the verification of the ORM model revealed that the object "
                             "has no properties \"%0\".")
                          .arg(propDescription->propertyName());
            result = false;
        }
    }

    return result;
}

QString IqOrmMetaModel::tableName() const
{
    return m_tableName;
}

void IqOrmMetaModel::setNotNull(const QString &property, bool notNull)
{
    IqOrmPropertyDescription *propDescr = const_cast<IqOrmPropertyDescription *>(propertyDescription(property));
    Q_CHECK_PTR(propDescr);
    Q_ASSERT_X(propDescr->mappedType() == IqOrmPropertyDescription::OneToOne ||
               propDescr->mappedType() == IqOrmPropertyDescription::ManyToOne,
               Q_FUNC_INFO,
               tr("Not null only supported one-to-one and many-to-one.").toLocal8Bit().constData());

    propDescr->setNotNull(notNull);
}

void IqOrmMetaModel::setReadOnly(const QString &property, bool readOnly)
{
    IqOrmPropertyDescription *propDescr = const_cast<IqOrmPropertyDescription *>(propertyDescription(property));
    Q_CHECK_PTR(propDescr);

    propDescr->setReadOnly(readOnly);
}

QStringList IqOrmMetaModel::sourcesIdList() const
{
    return QStringList() << tableName();
}
