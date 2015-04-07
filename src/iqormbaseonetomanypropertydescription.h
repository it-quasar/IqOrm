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

#ifndef IQORMBASEONETOMANYPROPERTYDESCRIPTION_H
#define IQORMBASEONETOMANYPROPERTYDESCRIPTION_H

#include "iqormmanyobjectdescribingpropertydescription.h"
#include "iqorminversedmappedpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmBaseOneToManyPropertyDescription :
        public IqOrmInversedMappedPropertyDescription,
        public IqOrmManyObjectDescribingPropertyDescription
{
    Q_OBJECT
public:
    explicit IqOrmBaseOneToManyPropertyDescription(IqOrmMetaModel *parent = Q_NULLPTR);

    virtual IqOrmPropertyDescription::MappedType mappedType() const Q_DECL_OVERRIDE;

    virtual IqOrmPropertyDescription::StoredValue storedValue() const Q_DECL_OVERRIDE;
};

#endif // IQORMBASEONETOMANYPROPERTYDESCRIPTION_H
