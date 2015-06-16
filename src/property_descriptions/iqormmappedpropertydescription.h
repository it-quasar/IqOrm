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

#ifndef IQORMMAPPEDPROPERTYDESCRIPTION_H
#define IQORMMAPPEDPROPERTYDESCRIPTION_H

#include "iqormpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmMappedPropertyDescription : public IqOrmPropertyDescription
{
    Q_OBJECT
public:
    explicit IqOrmMappedPropertyDescription(IqOrmMetaModel *parent = Q_NULLPTR);

    virtual const IqOrmMetaModel* associatedObjectOrmModel() const = 0;

    virtual const IqOrmPropertyDescription *inversedPropertyDescription() const = 0;
};

#endif // IQORMMAPPEDPROPERTYDESCRIPTION_H
