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

#ifndef IQORMMETAMODEL_H
#error 'iqormmetamoedel_impl.h' is not supposed to be included directly. Include 'iqormmetamodel.h' instead.
#endif

template <class T>
void IqOrmMetaModel::setOneToOne(const QString &property,
                             const QString &joinColumnName)
{
    Q_ASSERT(!property.isEmpty());

    IqOrmOneToOnePropertyDescription<T> *newDescription = new IqOrmOneToOnePropertyDescription<T>(this);
    newDescription->setPropertyName(property);
    if (joinColumnName.isEmpty())
        newDescription->setJoinColumnName(property);
    else
        newDescription->setJoinColumnName(joinColumnName);

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}

template <class T>
void IqOrmMetaModel::setInversedOneToOne(const QString &property,
                                     const QString &mappedBy)
{
    Q_ASSERT(!property.isEmpty());
    Q_ASSERT(!mappedBy.isEmpty());

    IqOrmInversedOneToOnePropertyDescription<T> *newDescription = new IqOrmInversedOneToOnePropertyDescription<T>(this);
    newDescription->setPropertyName(property);
    newDescription->setMappedBy(mappedBy);

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}


template <class T>
void IqOrmMetaModel::setManyToOne(const QString &property,
                              const QString &joinColumnName)
{
    Q_ASSERT(!property.isEmpty());

    IqOrmManyToOnePropertyDescription<T> *newDescription = new IqOrmManyToOnePropertyDescription<T>(this);
    newDescription->setPropertyName(property);
    if (joinColumnName.isEmpty())
        newDescription->setJoinColumnName(property);
    else
        newDescription->setJoinColumnName(joinColumnName);

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}

template <class T>
void IqOrmMetaModel::setOneToMany(const QString &property,
                              const QString &mappedBy)
{
    Q_ASSERT(!property.isEmpty());
    Q_ASSERT(!mappedBy.isEmpty());

    IqOrmOneToManyPropertyDescription<T> *newDescription = new IqOrmOneToManyPropertyDescription<T>(this);
    newDescription->setPropertyName(property);
    newDescription->setMappedBy(mappedBy);

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}

template <class T>
void IqOrmMetaModel::setManyToMany(const QString &property,
                               const IqOrmJoinTable &joinTable)
{
    Q_ASSERT(!property.isEmpty());

    IqOrmManyToManyPropertyDescription<T> *newDescription = new IqOrmManyToManyPropertyDescription<T>(this);
    newDescription->setPropertyName(property);

    IqOrmJoinTable* newJoinTable = new IqOrmJoinTable(joinTable, newDescription);
    newDescription->setJoinTable(newJoinTable);

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}

template <class T>
void IqOrmMetaModel::setInversedManyToMany(const QString &property,
                                       const QString &mappedBy)
{
    Q_ASSERT(!property.isEmpty());
    Q_ASSERT(!mappedBy.isEmpty());

    IqOrmInversedManyToManyPropertyDescription<T> *newDescription = new IqOrmInversedManyToManyPropertyDescription<T>(this);
    newDescription->setPropertyName(property);
    newDescription->setMappedBy(mappedBy);

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}
