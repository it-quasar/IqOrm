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

#ifndef IQORMPROPERTYDESCRIPTION_H
#define IQORMPROPERTYDESCRIPTION_H

#include <QObject>
#include "iqorm_global.h"
#include <QMetaProperty>

class IqOrmObject;
class IqOrmMetaModel;
class IQORMSHARED_EXPORT IqOrmPropertyDescription : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString propertyName READ propertyName WRITE setPropertyName NOTIFY propertyNameChanged)
    Q_PROPERTY(bool notNull READ notNull WRITE setNotNull NOTIFY notNullChanged)
    Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly NOTIFY readOnlyChanged)
public:
    enum MappedType
    {
        Direct,
        OneToOne,
        InversedOneToOne,
        ManyToOne,
        OneToMany,
        ManyToMany,
        InversedManyToMany
    };

    enum StoredValue
    {
        SimpeVariant,
        ObjectPointer,
        ObjectPointerList
    };

    explicit IqOrmPropertyDescription(IqOrmMetaModel *parent = Q_NULLPTR);

    virtual ~IqOrmPropertyDescription();

    virtual MappedType mappedType() const = 0;

    virtual StoredValue storedValue() const = 0;

    QVariant value(const IqOrmObject *object) const;

    bool setValue(IqOrmObject *object, const QVariant &value) const;

    QObject *lastPropertyObject(const QObject *object) const;

public:
    inline QString propertyName() const {return m_propertyName;}
    void setPropertyName(const QString &name);

    QMetaProperty targetStaticMetaPropery() const;

    void setTargetStaticMetaObject(const QMetaObject *targetStaticMetaObject);

    bool notNull() const;
    void setNotNull(bool notNull);

    bool readOnly() const;
    void setReadOnly(bool readOnly);

signals:
    void propertyNameChanged();
    void notNullChanged();
    void readOnlyChanged();

private:
    void updateMetaPropertyIndex();

private:
    QString m_propertyName;
    const QMetaObject *m_targetStaticMetaObject;
    int m_metaPropertyIndex;
    bool m_notNull;
    bool m_readOnly;
};

#endif // IQORMPROPERTYDESCRIPTION_H
