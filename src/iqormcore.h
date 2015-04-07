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

#ifndef IQORMCORE_H
#define IQORMCORE_H

#include <QObject>
#include <QReadWriteLock>
#include <QHash>
#include <QPointer>
#include "iqorm_global.h"

class IqOrmObject;
class IqOrmAbstractSynchronizer;
class IqOrmAbstractDataSource;
class IqOrmAbstractTriggers;

class IQORMSHARED_EXPORT IqOrmCore : public QObject
{
    Q_OBJECT
public:
    static IqOrmCore *instance();

    static IqOrmAbstractDataSource* dataSource();
    static void setDataSource(IqOrmAbstractDataSource *dataSource);


    static IqOrmAbstractTriggers *defaultTriggers();
    static void setDefaultTriggers(IqOrmAbstractTriggers *defaultTriggers);

private:
    explicit IqOrmCore(QObject *parent = Q_NULLPTR);

    static IqOrmCore * m_instance;

    QReadWriteLock m_defaultTriggersLock;
    IqOrmAbstractTriggers *m_defaultTriggers;
    QPointer<IqOrmAbstractDataSource> m_dataSource;
    QReadWriteLock m_dataSourceReadWriteLock;
};
#endif // IQORMCORE_H
