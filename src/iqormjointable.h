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

#ifndef IQORMJOINTABLE_H
#define IQORMJOINTABLE_H

#include <QObject>
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmJoinTable : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString joinColumnName READ joinColumnName WRITE setJoinColumnName NOTIFY joinColumnNameChanged)
    Q_PROPERTY(QString inverseJoinColumnName READ inverseJoinColumnName WRITE setInverseJoinColumnName NOTIFY inverseJoinColumnNameChanged)
public:
    explicit IqOrmJoinTable(QObject *parent = Q_NULLPTR);

    explicit IqOrmJoinTable(const IqOrmJoinTable &other, QObject *parent = Q_NULLPTR);

    bool isValid() const;

public:
    QString name() const;
    void setName(const QString &name);

    QString joinColumnName() const;
    void setJoinColumnName(const QString &joinColumnName);

    QString inverseJoinColumnName() const;
    void setInverseJoinColumnName(const QString &inverseJoinColumnName);

signals:
    void nameChanged();
    void joinColumnNameChanged();
    void inverseJoinColumnNameChanged();

private:
    QString m_name;
    QString m_joinColumnName;
    QString m_inverseJoinColumnName;
};

#endif // IQORMJOINTABLE_H
