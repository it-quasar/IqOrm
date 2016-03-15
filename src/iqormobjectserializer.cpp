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

#include "iqormobjectserializer.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include "iqormmetamodel.h"
#include "iqormpropertydescription.h"
#include "iqormdirectpropertydescription.h"
#include "iqormobjectfactory.h"
#include "iqormobjectprivateaccessor.h"
#include "iqormmetamodelprivateaccessor.h"

using namespace IqOrmPrivate;

IqOrmObjectSerializer::IqOrmObjectSerializer(QObject *parent) :
    QObject(parent)
{
}

QString IqOrmObjectSerializer::serialize(const IqOrmObject *object, const Format format)
{
    //Кастим к QObject
    const QObject *qtObject = dynamic_cast<const QObject*>(object);
    Q_CHECK_PTR(qtObject);

    switch(format)
    {
    case JSON:
    {
        QJsonObject rootObject;
        rootObject.insert("objectType", QJsonValue(qtObject->metaObject()->className()));
        rootObject.insert("objectId", QJsonValue(object->objectId()));

        QJsonObject propertiesObject;

        const IqOrmMetaModel *objectModel = object->ormMetaModel();
        Q_CHECK_PTR(objectModel);
        foreach (const IqOrmPropertyDescription * propertyDescription, IqOrmMetaModelPrivateAccessor::propertyDescriptions(objectModel)) {
            Q_CHECK_PTR(propertyDescription);
            switch (propertyDescription->storedValue()) {
            case IqOrmPropertyDescription::SimpleVariant:
                propertiesObject.insert(propertyDescription->propertyName(), QJsonValue::fromVariant(propertyDescription->value(object)));
                break;
            case IqOrmPropertyDescription::ObjectPointer: {
                const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                        = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propertyDescription);
                Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
                propertiesObject.insert(propertyDescription->propertyName(), QJsonValue::fromVariant(oneObjectDescribingPropertyDescription->valueAsObjectId(object)));
                break;
            }
            case IqOrmPropertyDescription::ObjectPointerList: {
                const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                        = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propertyDescription);
                Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
                propertiesObject.insert(propertyDescription->propertyName(), QJsonValue::fromVariant(manyObjectDescribingPropertyDescription->valueAsObjectIds(object)));
                break;
            }
            }
        }

        rootObject.insert("properties", QJsonValue(propertiesObject));

        QJsonDocument document(rootObject);

        return QString(document.toJson());
    }
    }

    return "";
}

IqOrmObject* IqOrmObjectSerializer::deserialize(const QString &serializeData,
                                                QObject *parent,
                                                const Format format)
{
    switch (format) {
    case JSON:
    {
        QJsonDocument document;
        QJsonParseError error;
        document = QJsonDocument::fromJson(serializeData.toLocal8Bit(), &error);
        if (error.error != QJsonParseError::NoError) {
            qWarning() << tr("Error on desirialize object from JSON data. Error on parse JSON serialize data. Error: %0")
                          .arg(error.errorString());
            return Q_NULLPTR;
        }
        if (!document.isObject()) {
            qWarning() << tr("Error on desirialize object from JSON data. JSON document must be object");
            return Q_NULLPTR;
        }
        QJsonObject rootObject = document.object();
        if (!rootObject.contains("objectType")) {
            qWarning() << tr("Error on desirialize object from JSON data. JSON object not contain \"objectType\" key.");
            return Q_NULLPTR;
        }

        QString className = rootObject.value("objectType").toString();
        IqOrmObject *object = IqOrmObjectFactory::create(className, parent);
        if (!object) {
            qWarning() << tr("Error on desirialize object from JSON data. Error on create object from objec factory for class %0")
                          .arg(className);
            return Q_NULLPTR;
        }

        IqOrmObjectPrivateAccessor::setObjectId(object, rootObject.value("objectId").toInt(-1));

        QJsonObject propertiesObject = rootObject.value("properties").toObject();
        const IqOrmMetaModel *ormModel = object->ormMetaModel();
        Q_CHECK_PTR(ormModel);

        foreach (const QString &propertyName, propertiesObject.keys()) {
            const IqOrmPropertyDescription *propDescription = IqOrmMetaModelPrivateAccessor::propertyDescription(ormModel, propertyName);
            if (propDescription) {
                switch (propDescription->storedValue()) {
                case IqOrmPropertyDescription::SimpleVariant:
                    propDescription->setValue(object, propertiesObject.value(propertyName));
                    break;
                case IqOrmPropertyDescription::ObjectPointer: {
                    const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                            = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propDescription);
                    Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
                    QVariant value = propertiesObject.value(propertyName);
                    bool ok;
                    qint64 objecId = value.toLongLong(&ok);
                    if (!ok) {
                        qWarning() << tr("Can not convert value %1 for property %0 to qint64.")
                                      .arg(propertyName)
                                      .arg(value.toString());
                        continue;
                    }
                    oneObjectDescribingPropertyDescription->setValueFromObjectId(object, objecId);
                    break;
                }
                case IqOrmPropertyDescription::ObjectPointerList: {
                    const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                            = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propDescription);
                    Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
                    QVariant value = propertiesObject.value(propertyName);
                    if (!value.canConvert(QVariant::String)) {
                        qWarning() << tr("Can not convert value for property %0 to QString.")
                                      .arg(propertyName);
                        continue;
                    }
                    QString objectIdsString = value.toString();
                    QStringList objectIdsStringList = objectIdsString.split(',', QString::SkipEmptyParts);

                    QVariantList objectIdsVariantList;
                    foreach (const QString &string, objectIdsStringList) {
                        bool ok;
                        qint64 objectId = string.toLongLong(&ok);
                        if (!ok) {
                            qWarning() << tr("Can not convert value %1 for property %0 to qint64.")
                                          .arg(propertyName)
                                          .arg(string);
                            continue;
                        }
                        objectIdsVariantList << objectId;
                    }

                    manyObjectDescribingPropertyDescription->setValueFromObjectIds(object, objectIdsVariantList);
                    break;
                }
                }
            } else {
                qWarning() << tr("In class %0 not found property %1.")
                              .arg(IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(ormModel)->className())
                              .arg(propertyName);
            }
        }

        return object;
    }
    }

    return Q_NULLPTR;
}
