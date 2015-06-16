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

#ifndef IQORMMANYOBJECTDESCRIBINGPROPERTYDESCRIPTION_H
#define IQORMMANYOBJECTDESCRIBINGPROPERTYDESCRIPTION_H

#include "iqormpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmManyObjectDescribingPropertyDescription
{
public:    
    virtual QVariantList valueAsObjectIds(const IqOrmObject *object) const = 0;

    virtual bool setValueFromObjectIds(IqOrmObject *object,
                                       const QVariantList &objectIds) const = 0;

protected:
    template <class T>
    static QVariantList propertyValueAsObjectIds(const IqOrmPropertyDescription *propertyDescription,
                                          const IqOrmObject *object);

    template <class T>
    static bool setPropertyValueFromObjectIds(const IqOrmPropertyDescription *propertyDescription,
                                              IqOrmObject *object,
                                              const QVariantList &objectIds);
};

#include "iqormmanyobjectdescribingpropertydescription_impl.h"

#endif // IQORMMANYOBJECTDESCRIBINGPROPERTYDESCRIPTION_H
