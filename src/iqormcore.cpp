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

#include "iqormcore.h"
#include "iqormnulldatasource.h"
#include <QPointer>
#include <QDebug>
#include <QReadLocker>
#include <QWriteLocker>
#include "iqormabstracttriggers.h"

IqOrmCore *IqOrmCore::m_instance = Q_NULLPTR;
IqOrmCore::IqOrmCore(QObject *parent) :
    QObject(parent),
    m_defaultTriggers(Q_NULLPTR),
    m_dataSource(new IqOrmNullDataSource(this))
{
}

IqOrmAbstractTriggers *IqOrmCore::defaultTriggers()
{
    QReadLocker locker (&(instance()->m_defaultTriggersLock));
    return instance()->m_defaultTriggers;
}

void IqOrmCore::setDefaultTriggers(IqOrmAbstractTriggers *dafaultTriggers)
{
    QWriteLocker locker (&(instance()->m_defaultTriggersLock));
    if (instance()->m_defaultTriggers)
        instance()->m_defaultTriggers->deleteLater();
    dafaultTriggers->setParent(instance());
    instance()->m_defaultTriggers = dafaultTriggers;
}


IqOrmCore* IqOrmCore::instance()
{
    if (m_instance)
        return m_instance;

    m_instance = new IqOrmCore();
    return m_instance;
}

void IqOrmCore::setDataSource(IqOrmAbstractDataSource *dataSource)
{
    QWriteLocker locker(&(instance()->m_dataSourceReadWriteLock));
    if (instance()->m_dataSource)
        instance()->m_dataSource->deleteLater();
    dataSource->setParent(instance());
    instance()->m_dataSource = dataSource;
}

IqOrmAbstractDataSource * IqOrmCore::dataSource()
{
    QReadLocker locker(&(instance()->m_dataSourceReadWriteLock));
    return instance()->m_dataSource;
}
