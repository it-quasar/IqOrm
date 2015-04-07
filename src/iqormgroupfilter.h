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

#ifndef IQORMGROUPFILTER_H
#define IQORMGROUPFILTER_H

#include "iqormabstractfilter.h"
#include "iqorm_global.h"
#include <QQmlListProperty>

class IQORMSHARED_EXPORT IqOrmGroupFilter : public IqOrmAbstractFilter
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<IqOrmAbstractFilter> filters READ qmlFilters)

    Q_CLASSINFO("DefaultProperty", "filters")

public:
    enum GroupType
    {
        And,
        Or
    };

    explicit IqOrmGroupFilter(QObject *parent = Q_NULLPTR);

    explicit IqOrmGroupFilter(GroupType type, QObject *parent = Q_NULLPTR);

    ~IqOrmGroupFilter();

    void add(IqOrmAbstractFilter *filter);

    void remove(IqOrmAbstractFilter *filter);

    IqOrmAbstractFilter *at(const int index) const;

    void clear();

    int count() const;

    inline QList<IqOrmAbstractFilter*> toList() const {return m_filters;}

    QQmlListProperty<IqOrmAbstractFilter> qmlFilters();

public:
    GroupType type() const;
    void setType(const GroupType type);

private:
    QList<IqOrmAbstractFilter *> m_filters;
    GroupType m_type;
};

#endif // IQORMGROUPFILTER_H
