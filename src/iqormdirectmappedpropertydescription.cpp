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

#include "iqormdirectmappedpropertydescription.h"
#include "iqormmetamodel.h"
#include "iqorminversedmappedpropertydescription.h"

IqOrmDirectMappedPropertyDescription::IqOrmDirectMappedPropertyDescription(IqOrmMetaModel *parent) :
    IqOrmMappedPropertyDescription(parent),
    m_inversedPropertyDescriptionAlreadyLooking(false),
    m_inversedPropertyDescription(Q_NULLPTR)
{
}

const IqOrmPropertyDescription *IqOrmDirectMappedPropertyDescription::inversedPropertyDescription() const
{
    if (m_inversedPropertyDescriptionAlreadyLooking)
        return m_inversedPropertyDescription;

    m_inversedPropertyDescriptionAlreadyLooking = true;
    const IqOrmMetaModel *ormModel = associatedObjectOrmModel();
    Q_CHECK_PTR(ormModel);
    foreach (const IqOrmPropertyDescription *propertyDescription, ormModel->propertyDescriptions()) {
        const IqOrmInversedMappedPropertyDescription *inversedProperty = qobject_cast<const IqOrmInversedMappedPropertyDescription *>(propertyDescription);
        if (!inversedProperty)
            continue;
        if (inversedProperty->mappedBy() == propertyName()) {
            m_inversedPropertyDescription = inversedProperty;
            break;
        }
    }

    return m_inversedPropertyDescription;
}
