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

#ifndef IQORMJOINCOLUMNDIRECTMAPPEDPROPERTYDESCRIPTION_H
#define IQORMJOINCOLUMNDIRECTMAPPEDPROPERTYDESCRIPTION_H

#include "iqormdirectmappedpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmJoinColumnDirectMappedPropertyDescription : public IqOrmDirectMappedPropertyDescription
{
    Q_OBJECT
    Q_PROPERTY(QString joinColumnName READ joinColumnName WRITE setJoinColumnName NOTIFY joinColumnNameChanged)
public:
    explicit IqOrmJoinColumnDirectMappedPropertyDescription(IqOrmMetaModel *parent = Q_NULLPTR);

public:
    QString joinColumnName() const;
    void setJoinColumnName(const QString &joinColumnName);

signals:
    void joinColumnNameChanged();

private:
    QString m_joinColumnName;
};

#endif // IQORMJOINCOLUMNDIRECTMAPPEDPROPERTYDESCRIPTION_H
