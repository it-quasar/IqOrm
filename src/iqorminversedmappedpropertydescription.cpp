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

#include "iqorminversedmappedpropertydescription.h"
#include "iqormmetamodel.h"

IqOrmInversedMappedPropertyDescription::IqOrmInversedMappedPropertyDescription(IqOrmMetaModel *parent) :
    IqOrmMappedPropertyDescription(parent),
    m_inversedPropertyDescriptionAlreadyLooking(false),
    m_inversedPropertyDescription(Q_NULLPTR)
{
}

const IqOrmPropertyDescription *IqOrmInversedMappedPropertyDescription::inversedPropertyDescription() const
{
    if (m_inversedPropertyDescriptionAlreadyLooking)
        return m_inversedPropertyDescription;

    m_inversedPropertyDescriptionAlreadyLooking = true;
    const IqOrmMetaModel *ormModel = associatedObjectOrmModel();
    Q_CHECK_PTR(ormModel);
    Q_ASSERT(!mappedBy().isEmpty());
    m_inversedPropertyDescription = ormModel->propertyDescription(mappedBy());
    return m_inversedPropertyDescription;
}

QString IqOrmInversedMappedPropertyDescription::mappedBy() const
{
    return m_mappedBy;
}

void IqOrmInversedMappedPropertyDescription::setMappedBy(const QString &mappedBy)
{
    if (m_mappedBy != mappedBy) {
        m_mappedBy = mappedBy;
        emit mappedByChanged();
    }
}
