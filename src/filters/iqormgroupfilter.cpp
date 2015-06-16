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

#include "iqormgroupfilter.h"

IqOrmGroupFilter::IqOrmGroupFilter(QObject *parent) :
    IqOrmAbstractFilter(parent),
    m_type(Or)
{
}

IqOrmGroupFilter::IqOrmGroupFilter(GroupType type, QObject *parent) :
    IqOrmAbstractFilter(parent),
    m_type(type)
{
}

IqOrmGroupFilter::~IqOrmGroupFilter()
{

}

QQmlListProperty<IqOrmAbstractFilter> IqOrmGroupFilter::qmlFilters()
{
    return QQmlListProperty<IqOrmAbstractFilter>(this, m_filters);
}

IqOrmGroupFilter::GroupType IqOrmGroupFilter::type() const
{
    return m_type;
}

int IqOrmGroupFilter::count() const
{
    return m_filters.count();
}

void IqOrmGroupFilter::add(IqOrmAbstractFilter *filter)
{
    if (!filter)
        return;
    filter->setParent(this);

    m_filters.append(filter);
}

void IqOrmGroupFilter::remove(IqOrmAbstractFilter *filter)
{
    if (!filter)
        return;
    filter->setParent(Q_NULLPTR);

    m_filters.removeAll(filter);
}

IqOrmAbstractFilter* IqOrmGroupFilter::at(const int index) const
{
    Q_ASSERT(index >= 0);
    Q_ASSERT(index < m_filters.count());

    return m_filters.at(index);
}

void IqOrmGroupFilter::clear()
{
    qDeleteAll(m_filters);
    m_filters.clear();
}

void IqOrmGroupFilter::setType(const GroupType type)
{
    if (m_type != type) {
        m_type = type;
    }
}
