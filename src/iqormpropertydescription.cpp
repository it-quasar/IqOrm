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

#include "iqormpropertydescription.h"
#include "iqormobject.h"

IqOrmPropertyDescription::IqOrmPropertyDescription(IqOrmMetaModel *parent) :
    QObject(parent),
    m_propertyName(""),
    m_targetStaticMetaObject(Q_NULLPTR),
    m_notNull(false),
    m_readOnly(false)
{
}

IqOrmPropertyDescription::~IqOrmPropertyDescription()
{
}

QObject *IqOrmPropertyDescription::lastPropertyObject(const QObject *object) const
{
    Q_CHECK_PTR(object);

    QObject *lastObject = const_cast<QObject *>(object);
    for (int i = 0; i < m_propertyPath.count() - 1; i++) {
        lastObject = lastObject->property(m_propertyPath[i].toLocal8Bit().constData()).value<QObject *>();
        Q_ASSERT_X(lastObject,
                   Q_FUNC_INFO,
                   tr("Not found object in propery \"%0\" of class \"%1\".")
                   .arg(propertyName())
                   .arg(m_targetStaticMetaObject->className())
                   .toLocal8Bit().constData());
    }

    return lastObject;
}

void IqOrmPropertyDescription::setPropertyName(const QString &name)
{
    if (m_propertyName != name)
    {
        m_propertyName = name;

        emit propertyNameChanged();
        updateMetaProperty();

        m_propertyPath = m_propertyName.split('.');
        m_lastPropertyName = m_propertyPath.last();
    }
}

void IqOrmPropertyDescription::setTargetStaticMetaObject(const QMetaObject *targetStaticMetaObject)
{
    if (m_targetStaticMetaObject != targetStaticMetaObject) {
        m_targetStaticMetaObject = targetStaticMetaObject;
        updateMetaProperty();
    }
}

QMetaProperty IqOrmPropertyDescription::targetStaticMetaPropery() const
{
    return m_targetStaticMetaProperty;
}

void IqOrmPropertyDescription::updateMetaProperty()
{
    if (m_targetStaticMetaObject) {
        int index = m_targetStaticMetaObject->indexOfProperty(m_propertyName.toLocal8Bit().constData());
        m_targetStaticMetaProperty = m_targetStaticMetaObject->property(index);
    }
}
bool IqOrmPropertyDescription::readOnly() const
{
    return m_readOnly;
}

void IqOrmPropertyDescription::setReadOnly(bool readOnly)
{
    if (m_readOnly != readOnly) {
        m_readOnly = readOnly;
        emit readOnlyChanged();
    }
}

bool IqOrmPropertyDescription::notNull() const
{
    return m_notNull;
}

void IqOrmPropertyDescription::setNotNull(bool notNull)
{
    if (m_notNull != notNull) {
        m_notNull = notNull;
        emit notNullChanged();
    }
}

QVariant IqOrmPropertyDescription::value(const IqOrmObject *object) const
{
    Q_CHECK_PTR(object);
    const QObject *qobject = dynamic_cast<const QObject*>(object);
    Q_CHECK_PTR(qobject);

    if (m_propertyPath.count() == 1)
        return m_targetStaticMetaProperty.read(qobject);

    const QObject *lastPropObj = lastPropertyObject(qobject);
    Q_CHECK_PTR (lastPropObj);
    return lastPropObj->property(m_lastPropertyName.toLocal8Bit().constData());
}

bool IqOrmPropertyDescription::setValue(IqOrmObject *object, const QVariant &value) const
{
    Q_CHECK_PTR(object);
    QObject *qobject = dynamic_cast<QObject*>(object);
    Q_CHECK_PTR(qobject);

    if (m_propertyPath.count() == 1)
        return m_targetStaticMetaProperty.write(qobject, value);

    QObject *lastPropObj = lastPropertyObject(qobject);
    Q_CHECK_PTR (lastPropObj);
    return lastPropObj->setProperty(m_lastPropertyName.toLocal8Bit().constData(), value);
}
