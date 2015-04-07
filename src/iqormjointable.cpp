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

#include "iqormjointable.h"

IqOrmJoinTable::IqOrmJoinTable(QObject *parent) :
    QObject(parent)
{
}

IqOrmJoinTable::IqOrmJoinTable(const IqOrmJoinTable &other, QObject *parent) :
    QObject(parent),
    m_name(other.name()),
    m_joinColumnName(other.joinColumnName()),
    m_inverseJoinColumnName(other.inverseJoinColumnName())
{
}

bool IqOrmJoinTable::isValid() const
{
    return !m_name.isEmpty()
            && !m_joinColumnName.isEmpty()
            && !m_inverseJoinColumnName.isEmpty();
}

QString IqOrmJoinTable::name() const
{
    return m_name;
}

void IqOrmJoinTable::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

QString IqOrmJoinTable::joinColumnName() const
{
    return m_joinColumnName;
}

void IqOrmJoinTable::setJoinColumnName(const QString &joinColumnName)
{
    if (m_joinColumnName != joinColumnName) {
        m_joinColumnName = joinColumnName;
        emit joinColumnNameChanged();
    }
}

QString IqOrmJoinTable::inverseJoinColumnName() const
{
    return m_inverseJoinColumnName;
}

void IqOrmJoinTable::setInverseJoinColumnName(const QString &inverseJoinColumnName)
{
    if (m_inverseJoinColumnName != inverseJoinColumnName) {
        m_inverseJoinColumnName = inverseJoinColumnName;
        emit inverseJoinColumnNameChanged();
    }
}



