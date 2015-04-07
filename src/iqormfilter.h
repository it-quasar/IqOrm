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

#ifndef IQORMFILTER_H
#define IQORMFILTER_H

#include "iqormabstractfilter.h"
#include "iqorm_global.h"
#include <QVariant>

class IQORMSHARED_EXPORT IqOrmFilter : public IqOrmAbstractFilter
{
    Q_OBJECT
    Q_PROPERTY(Operation operation READ operation WRITE setOperation NOTIFY operationChanged)
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(Qt::CaseSensitivity caseSensitivity READ caseSensitivity WRITE setCaseSensitivity NOTIFY caseSensitivityChanged)

    Q_ENUMS(Operation)
public:
    enum Operation
    {
        None,
        Equals,
        NotEquals,
        GreaterThan,
        LessThan,
        GreaterOrEquals,
        LessOrEquals,
        StartsWith,
        EndsWith,
        Contains
    };

    explicit IqOrmFilter(QObject *parent = Q_NULLPTR);

    ~IqOrmFilter();

    explicit IqOrmFilter(const QString &property, Operation operation, const QVariant &value, QObject *parent = Q_NULLPTR);

public:

    inline Operation operation() const {return _operation;}

    void setOperation(const Operation operation);

    inline QString property() const {return _property;}

    void setProperty(const QString &property);

    inline QVariant value() const {return _value;}

    void setValue(const QVariant &value);

    inline Qt::CaseSensitivity caseSensitivity() const {return _caseSensitivity;}

    void setCaseSensitivity(const Qt::CaseSensitivity caseSensitivity);

signals:
    void operationChanged();
    void propertyChanged();
    void valueChanged();
    void caseSensitivityChanged();

private:
    Operation _operation;
    QString _property;
    QVariant _value;
    Qt::CaseSensitivity _caseSensitivity;
};

#endif // IQORMFILTER_H
