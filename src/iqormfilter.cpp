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
    _operation(None),
    _property(""),
    _value(QVariant()),
    _caseSensitivity(Qt::CaseSensitive)
{
}

IqOrmFilter::~IqOrmFilter()
{

}

IqOrmFilter::IqOrmFilter(const QString &property, Operation operation, const QVariant &value, QObject *parent) :
    IqOrmAbstractFilter(parent),
    _operation(operation),
    _property(property),
    _value(value),
    _caseSensitivity(Qt::CaseSensitive)
{
}

void IqOrmFilter::setOperation(const Operation operation)
{
    if (_operation == operation)
        return;
    _operation = operation;

    emit operationChanged();
}

void IqOrmFilter::setProperty(const QString &property)
{
    if (_property == property)
        return;
    _property = property;

    emit propertyChanged();
}

void IqOrmFilter::setValue(const QVariant &value)
{
    if (_value == value)
        return;
    _value = value;

    emit valueChanged();
}

void IqOrmFilter::setCaseSensitivity(const Qt::CaseSensitivity caseSensitivity)
{
    if (_caseSensitivity == caseSensitivity)
        return;
    _caseSensitivity = caseSensitivity;

    emit caseSensitivityChanged();
}
