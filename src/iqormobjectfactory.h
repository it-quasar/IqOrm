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

#ifndef IQORMOBJECTFACTORY_H
#define IQORMOBJECTFACTORY_H

#include <QObject>
#include <QHash>
#include "iqorm_global.h"

class IqOrmObject;

class IQORMSHARED_EXPORT IqOrmObjectFactory : public QObject
{
    Q_OBJECT
public:
    explicit IqOrmObjectFactory(QObject *parent = Q_NULLPTR);

    static IqOrmObject *create(const QString &className, QObject *parent = Q_NULLPTR);

    template <class T>
    static void registerClass();

private:
    class IqOrmObjectBaseBulder
    {
    public:
        virtual IqOrmObject *build(QObject *parent = Q_NULLPTR) const = 0;
    };

    template <class T>
    class IqOrmObjectBulder : public IqOrmObjectBaseBulder
    {
        virtual IqOrmObject *build(QObject *parent = Q_NULLPTR) const Q_DECL_OVERRIDE;
    };

    static QHash<QString, IqOrmObjectBaseBulder *> m_builders;
};

#include "iqormobjecfactory_impl.h"

#endif // IQORMOBJECTFACTORY_H
