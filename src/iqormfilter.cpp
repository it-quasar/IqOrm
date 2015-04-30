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

#include "iqormfilter.h"

IqOrmFilter::IqOrmFilter(QObject *parent) :
    IqOrmAbstractFilter(parent),
    m_condition(None),
    m_property(""),
    m_value(QVariant()),
    m_caseSensitivity(Qt::CaseSensitive)
{
}

IqOrmFilter::~IqOrmFilter()
{

}

IqOrmFilter::IqOrmFilter(const QString &property, Condition operation, const QVariant &value, QObject *parent) :
    IqOrmAbstractFilter(parent),
    m_condition(operation),
    m_property(property),
    m_value(value),
    m_caseSensitivity(Qt::CaseSensitive)
{
}

IqOrmFilter::Condition IqOrmFilter::condition() const
{
    return m_condition;
}

void IqOrmFilter::setCondition(const Condition operation)
{
    if (m_condition == operation)
        return;
    m_condition = operation;

    emit conditionChanged();
}

QString IqOrmFilter::property() const
{
    return m_property;
}

void IqOrmFilter::setProperty(const QString &property)
{
    if (m_property == property)
        return;
    m_property = property;

    emit propertyChanged();
}

QVariant IqOrmFilter::value() const
{
    return m_value;
}

void IqOrmFilter::setValue(const QVariant &value)
{
    if (m_value == value)
        return;
    m_value = value;

    emit valueChanged();
}

Qt::CaseSensitivity IqOrmFilter::caseSensitivity() const
{
    return m_caseSensitivity;
}

void IqOrmFilter::setCaseSensitivity(const Qt::CaseSensitivity caseSensitivity)
{
    if (m_caseSensitivity == caseSensitivity)
        return;
    m_caseSensitivity = caseSensitivity;

    emit caseSensitivityChanged();
}
