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

#ifndef IQORMOBJECTPRIVATEACCESSOR_H
#define IQORMOBJECTPRIVATEACCESSOR_H

#include <QVariant>
#include "iqorm_global.h"

class IqOrmObject;
class IqOrmPropertyDescription;
class IqOrmObjectRawData;
class IqOrmAbstractTriggers;

namespace IqOrmPrivate {

class IQORMSHARED_EXPORT IqOrmObjectPrivateAccessor
{
public:
    static void setObjectId(IqOrmObject *object, const int id);

    static void setVaules(IqOrmObject *object, const IqOrmObjectRawData &rawData);

    static void updateObjectSourceProperites(IqOrmObject *object);

    static void resetObject(IqOrmObject *object);

    static QVariant sourcePropertyValue(const IqOrmObject *object,
                                        const IqOrmPropertyDescription *propertyDescription);

    static bool isObjectLoadedFromDataSource(const IqOrmObject *object);
    static void setObjectIsLoadedFromDataSource(IqOrmObject *object,
                                          bool isLoadedFromDataSource);

    static bool isObjectSavedToDataSource(const IqOrmObject *object);

    static IqOrmAbstractTriggers *triggers(const IqOrmObject *object);

    enum class Extensions {
        Null
    };

    static bool isObjectIqOrmExtensionEnabled(const IqOrmObject *object, Extensions extension);
};

}

#endif // IQORMOBJECTPRIVATEACCESSOR_H
