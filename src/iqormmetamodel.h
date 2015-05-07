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

#ifndef IQORMMETAMODEL_H
#define IQORMMETAMODEL_H

#include <QObject>
#include "iqorm_global.h"

#include "iqormdirectpropertydescription.h"
#include "iqormonetoonepropertydescription.h"
#include "iqorminversedonetoonepropertydescription.h"
#include "iqormmanytoonepropertydescription.h"
#include "iqormonetomanypropertydescription.h"
#include "iqormmanytomanypropertydescription.h"
#include "iqorminversedmanytomanypropertydescription.h"
#include "iqormjointable.h"

class IqOrmObject;
class IqOrmQmlObject;
class IqOrmAbstractTriggers;

class IQORMSHARED_EXPORT IqOrmMetaModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString tableName READ tableName WRITE setTableName NOTIFY tableNameChanged)
    Q_PROPERTY(IqOrmAbstractTriggers* triggers READ triggers WRITE setTriggers NOTIFY triggersChanged)
    Q_CLASSINFO("DefaultProperty", "propertyDescriptions")
public:
    explicit IqOrmMetaModel(QObject *parent = Q_NULLPTR);

    void setPropertyColumn(const QString &property,
                           const QString &columnName = QString());

    template <class T>
    void setOneToOne(const QString &property,
                     const QString &joinColumnName = QString());

    template <class T>
    void setInversedOneToOne(const QString &property,
                             const QString &mappedBy);

    template <class T>
    void setManyToOne(const QString &property,
                      const QString &joinColumnName = QString());

    template <class T>
    void setOneToMany(const QString &property,
                      const QString &mappedBy);

    template <class T>
    void setManyToMany(const QString &property,
                       const IqOrmJoinTable &joinTable);

    template <class T>
    void setInversedManyToMany(const QString &property,
                               const QString &mappedBy);


    void setNotNull(const QString &property,
                    bool notNull = true);

    void setReadOnly(const QString &property,
                     bool readOnly = true);

    QStringList sourcesIdList() const;

    QSet<const IqOrmPropertyDescription *> propertyDescriptions() const;

    const IqOrmPropertyDescription * propertyDescription(const QString &propertyName) const;

    bool isValid() const;

public:
    QString tableName() const;
    void setTableName(const QString tableName);

    const QMetaObject *targetStaticMetaObject() const;
    void setTargetStaticMetaObject(const QMetaObject *targetStaticMetaObject);

    IqOrmAbstractTriggers *triggers() const;
    void setTriggers(IqOrmAbstractTriggers *triggers);

signals:
    void tableNameChanged();
    void triggersChanged();

private:
    bool addPropertyOrmDescription(IqOrmPropertyDescription *description);
    bool checkPropertyDescriptionIsValid(const IqOrmPropertyDescription *propertyDescription) const;

private:
    friend class IqOrmObject;
    friend class IqOrmQmlObject;

    QSet<const IqOrmPropertyDescription *> m_propertyDescriptions;

    QString m_tableName;
    const QMetaObject *m_targetStaticMetaObject;

    IqOrmAbstractTriggers *m_triggers;

    //Объект для валидации модели
    const QObject *m_validationObject;
};

#include "iqormmetamodel_impl.h"

#endif // IQORMMETAMODEL_H
