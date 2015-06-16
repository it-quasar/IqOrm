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

#ifndef IQORMSQLMAPPEDPROPERTYDESCRIPTIONPROCESSOR_H
#define IQORMSQLMAPPEDPROPERTYDESCRIPTIONPROCESSOR_H

#include "iqormsqlabstractpropertydescriptionprocessor.h"
#include "iqormmappedpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlMappedPropertyDescriptionProcessor : public IqOrmSqlAbstractPropertyDescriptionProcessor
{
public:
    explicit IqOrmSqlMappedPropertyDescriptionProcessor();

    const IqOrmMappedPropertyDescription *propertyDescription() const;

protected:
    const IqOrmMetaModel *associatedOrmModel() const;
};

#endif // IQORMSQLMAPPEDPROPERTYDESCRIPTIONPROCESSOR_H
