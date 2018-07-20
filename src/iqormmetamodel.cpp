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

#include "iqormmetamodel.h"
#include "iqormdirectpropertydescription.h"
#include "iqormpropertydescription.h"
#include "iqormobject.h"
#include "iqormonetoonepropertydescription.h"
#include "iqormmanytoonepropertydescription.h"
#include "iqormobjectfactory.h"
#include <QMetaProperty>
#include <QHash>
#include <QDebug>
#include "iqormabstracttriggers.h"

/*!
 * \class IqOrmMetaModel
 * \inmodule IqOrm
 *
 * \brief Мета-модель, используемая для описания способа хранения объекта ORM в источнике данных.
 *
 * Для того, чтобы IqOrm знала как сохранить объект ORM в источнике данных, она использует мета-модель. Мета-модель
 * описывает всю необходимую информацию, для сохранения объекта в источнике данных.
 *
 * Мета-модель создается при запуске приложения для каждого класса объектов ORM и инициализируется вызовом переопределенных
 * методов IqOrmObject::initializeOrmMetaModel(). В данный метод передается указатель на мета-модель, которую нужно инициализировать.
 * Для каждого класса объекта ORM вызов метода IqOrmObject::initializeOrmMetaModel() будет происходить один раз.
 *
 * \snippet IqOrmMetaModel/user.cpp initializeOrmMetaModel
 *
 * Для любого объекта ORM необходимо указать имя таблицы в свойстве \l tableName, в которой будет сохраняться записи для данного объекта. IqOrm
 * использует шаблон Active Record, поэтому для каждого класса объектов ORM используется своя таблица в источнике
 * данных, для сохранения значений объектов. Возможно хранить несколько классов объектов ORM в одной таблице, если
 * эти классы наследуются друг от друга, но в этом случае, необходимо четко понимать, что Вы делаете.
 *
 * Так же необходимо описать свойства объекта ORM. Свойства могут быть либо прямыми/простыми, либо связаные.
 *
 * Прямые (простые) свойства - это такие свойства, которые непосредственно отображаются в запись таблицы объекта источника данных. Это
 * обычно свойства, содержащие простые типы, напимер,: int, QString, float, bool и т.д. Данные свойства описываются методом
 * setDirect().
 *
 * Связаные свойства - это такие свойства, которые содержат в себе связь с другим объектом. Данные свойства не всегда
 * непосредственно отображаются в запись таблицы объекта источника данных, их значения может содержаться в таблицах
 * связаных объектов, либо вообще в отдельных таблицах. IqOrm поддерживает следующие связи с другими объектами в качестве
 * свойств:
 * \list
 *     \li один-к-одному, устанавливается методом setOneToOne() и setInversedOneToOne();
 *     \li один-ко-многому, устанавливается методом setOneToMany();
 *     \li много-к-одному, устанавливается методом setManyToOne();
 *     \li мноие-к-многому, устанавливается методом setManyToMany() и setInversedManyToMany().
 * \endlist
 *
 * Подробнее об использовании связаных свойств читайте в {object mapped}.
 *
 * Так же для любого свойства можно указать, что свойство доступно только для чтения, используя метод setReadOnly(), и
 * что свойство не может содержать NULL, используя метод setNotNull(). По-умолчанию, свойства доступны для записи и могут
 * принимать NULL.
 *
 * Для определенных классов объектов ORM могут понадобится специальные тригеры, отличные от общесистемных, заданных в
 * IqOrmCore::defaultTriggers(). Такого поведения можно добится, используя свойство \l triggers, установив для мето-моделей
 * нужных классов объектов ORM специальные треггеры.
 *
 * \sa IqOrmObject
 */


/*!
 * Создает мета-модель с родителем \a parent.
 */
IqOrmMetaModel::IqOrmMetaModel(QObject *parent) :
    QObject(parent),
    m_tableName(""),
    m_targetStaticMetaObject(Q_NULLPTR),
    m_triggers(Q_NULLPTR),
    m_validationObject(Q_NULLPTR),
    m_ready(false)
{
}

void IqOrmMetaModel::setTableName(const QString tableName)
{
    if (m_tableName != tableName)
    {
        m_tableName = tableName;

        emit tableNameChanged();
    }
}

bool IqOrmMetaModel::checkPropertyDescriptionIsValid(const IqOrmPropertyDescription *propertyDescription) const
{
    Q_CHECK_PTR(m_validationObject);

    //Попробуем найти свойство у данного объекта

    const QObject *lastObject = propertyDescription->lastPropertyObject(m_validationObject);

    if (!lastObject) {
        return false;
    }

    const QString lastPropName = propertyDescription->propertyName().split('.').last();

    int propertyIntex = lastObject->metaObject()->indexOfProperty(lastPropName.toLocal8Bit().constData());
    return propertyIntex != -1;
}

bool IqOrmMetaModel::isReady() const
{
    return m_ready;
}

void IqOrmMetaModel::setReady(bool ready)
{
    m_ready = ready;
}

/*!
 * \property IqOrmMetaModel::triggers
 *
 * Данное свойство содержит указатель на специальные тригеры, которые будут вызваться при работе с объектами ORM,
 * класса, которому принадлежит мета-модель.
 *
 * Тригеры позволяют выполнять определенные действия перед операциями, которые работают с источником данных, в транзакции
 * операции.
 *
 * Для всех объектов ORM, у которых не переопределен метод IqOrmObject::triggers(), тригеры используются в следующем порядке:
 *     1. Тригеры, указаные для мета-модели объекта (см. IqOrmMetaModel::triggers());
 *     2. Тригеры, используемы по умолчанию (см. IqOrmCore::defaultTriggers()).
 *
 * Более подробно о тригерах рассказано в \l {work with triggers}.
 *
 * При установки тригера для мета-модели, мета-модель автоматически становится родителем тригера.
 */

IqOrmAbstractTriggers *IqOrmMetaModel::triggers() const
{
    return m_triggers;
}

void IqOrmMetaModel::setTriggers(IqOrmAbstractTriggers *triggers)
{
    if (m_triggers != triggers) {
        if (m_triggers)
            m_triggers->deleteLater();
        if (triggers)
            triggers->setParent(this);
        m_triggers = triggers;
        emit triggersChanged();
    }
}


bool IqOrmMetaModel::addPropertyOrmDescription(IqOrmPropertyDescription *description)
{
    Q_CHECK_PTR (description);
    Q_ASSERT (!description->propertyName().isEmpty());

    //Если для данного свойства уже есть описание
    foreach (const IqOrmPropertyDescription *oldDescription, m_propertyDescriptions)
    {
        Q_CHECK_PTR (oldDescription);
        if (oldDescription->propertyName() == description->propertyName()) {
            IqOrmPropertyDescription *notConstOldDescription = const_cast<IqOrmPropertyDescription *>(oldDescription);
            notConstOldDescription->deleteLater();
            m_propertyDescriptions.remove(oldDescription);
            break;
        }
    }

    //Проверим, что свойство правильное
    Q_ASSERT_X(checkPropertyDescriptionIsValid(description),
               Q_FUNC_INFO,
               tr("IqOrm: Property \"%0\" no foud in %1. Check Q_PROPERTY.")
               .arg(description->propertyName())
               .arg(m_validationObject->metaObject()->className()).toLocal8Bit().constData());

    description->setParent(this);
    description->setTargetStaticMetaObject(targetStaticMetaObject());
    m_propertyDescriptions << description;

    return true;
}

/*!
 * Устанавливает для прямого(простого) свойства \a property колонку для хранения \a columnName. Если \a columnName равен
 * пустой строке, то в качестве имени колонки будет использоваться имя свойства \a property.
 *
 * Прямые (простые) свойства - это такие свойства, которые непосредственно отображаются в одну колону записи таблицы объекта
 * источника данных.
 *
 * IqOrm поддерживат следующие типы в качестве прямых свойств:
 * \list
 *      \li все численные типы (int, float, qint64 и т.д.);
 *      \li строки в виде QString, QLatin1String;
 *      \li время в виде QTime;
 *      \li дату в виде QDate;
 *      \li время и дату в виде QDateTime;
 *      \li списки строк в виде QStringList.
 * \endlist
 *
 * \deprecated
 * Вместо данного метода необходимо использовать метод setDirect().
 */
void IqOrmMetaModel::setPropertyColumn(const QString &property,
                                   const QString &columnName)
{
    setDirect(property, columnName);
}

/*!
 * Устанавливает для прямого(простого) свойства \a property колонку для хранения \a columnName. Если \a columnName равен
 * пустой строке, то в качестве имени колонки будет использоваться имя свойства \a property.
 *
 * Прямые (простые) свойства - это такие свойства, которые непосредственно отображаются в одну колону записи таблицы объекта
 * источника данных.
 *
 * IqOrm поддерживат следующие типы в качестве прямых свойств:
 * \list
 *      \li все численные типы (int, float, qint64 и т.д.);
 *      \li строки в виде QString, QLatin1String;
 *      \li время в виде QTime;
 *      \li дату в виде QDate;
 *      \li время и дату в виде QDateTime;
 *      \li списки строк в виде QStringList.
 * \endlist
 */
void IqOrmMetaModel::setDirect(const QString &property, const QString &columnName)
{
    Q_ASSERT(!property.isEmpty());

    IqOrmDirectPropertyDescription *newDescription = new IqOrmDirectPropertyDescription(this);
    newDescription->setPropertyName(property);
    if (columnName.isEmpty()) {
        newDescription->setColumnName(property);
    } else {
        newDescription->setColumnName(columnName);
    }

    if (!addPropertyOrmDescription(newDescription))
        newDescription->deleteLater();
}

QSet<const IqOrmPropertyDescription *> IqOrmMetaModel::propertyDescriptions() const
{
    return m_propertyDescriptions;
}

//QQmlListProperty<IqOrmPropertyDescription> IqOrmMetaModel::qmlPropertyDescription()
//{
//    return QQmlListProperty<IqOrmPropertyDescription>(this,
//                                                      &m_propertyDescriptions,
//                                                      &qmlPropertyDescriptionAppend,
//                                                      &qmlPropertyDescriptionCount,
//                                                      &qmlPropertyDescriptionAt,
//                                                      &qmlPropertyDescriptionClear);
//}

//void IqOrmMetaModel::qmlPropertyDescriptionAppend(QQmlListProperty<IqOrmPropertyDescription> *property, IqOrmPropertyDescription *value)
//{
//    if (!property)
//        return;
//    if (!value)
//        return;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return;
//    model->addPropertyOrmDescription(value);
//}

//IqOrmPropertyDescription * IqOrmMetaModel::qmlPropertyDescriptionAt(QQmlListProperty<IqOrmPropertyDescription> *property, int index)
//{
//    if (!property)
//        return NULL;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return NULL;
//    if (index > 0 && index < model->m_propertyDescriptions.count())
//    {
//        return model->m_propertyDescriptions[index];
//    }
//    return NULL;
//}

//void IqOrmMetaModel::qmlPropertyDescriptionClear(QQmlListProperty<IqOrmPropertyDescription> *property)
//{
//    if (!property)
//        return;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return;
//    qDeleteAll(model->m_propertyDescriptions);
//    model->m_propertyDescriptions.clear();
//}

//int IqOrmMetaModel::qmlPropertyDescriptionCount(QQmlListProperty<IqOrmPropertyDescription> *property)
//{
//    if (!property)
//        return 0;
//    IqOrmMetaModel *model = qobject_cast<IqOrmMetaModel*>(property->object);
//    if (!model)
//        return 0;
//    return model->m_propertyDescriptions.count();
//}

const QMetaObject *IqOrmMetaModel::targetStaticMetaObject() const
{
    return m_targetStaticMetaObject;
}

void IqOrmMetaModel::setTargetStaticMetaObject(const QMetaObject *targetStaticMetaObject)
{
    if (m_targetStaticMetaObject != targetStaticMetaObject) {
        m_targetStaticMetaObject = targetStaticMetaObject;

        if (m_targetStaticMetaObject) {
            //Создадим новый объект валидации
            if (m_validationObject)
                delete m_validationObject;

            m_validationObject = dynamic_cast<QObject*>(IqOrmObjectFactory::create(targetStaticMetaObject->className(), this));
        }

        //Установим метообъект всем свойствам
        foreach (const IqOrmPropertyDescription *propDescription, m_propertyDescriptions) {
            Q_CHECK_PTR(propDescription);
            IqOrmPropertyDescription *notConstPropertyDescription = const_cast<IqOrmPropertyDescription *>(propDescription);
            notConstPropertyDescription->setTargetStaticMetaObject(targetStaticMetaObject);
        }
    }
}


const IqOrmPropertyDescription *IqOrmMetaModel::propertyDescription(const QString &propertyName) const
{
    foreach (const IqOrmPropertyDescription *description, m_propertyDescriptions)
    {
        Q_CHECK_PTR (description);
        if (description->propertyName() == propertyName)
            return description;
    }

    return NULL;
}

const IqOrmPropertyDescription *IqOrmMetaModel::propertyDescriptionOnNotifySignal(int propertyNotifySignalIndex) const
{
        foreach (const IqOrmPropertyDescription *description, m_propertyDescriptions)
        {
            Q_CHECK_PTR (description);
            if (description->targetStaticMetaPropery().notifySignalIndex() == propertyNotifySignalIndex)
                return description;
        }

    return NULL;
}

bool IqOrmMetaModel::isValid() const
{
    bool result = true;

    Q_CHECK_PTR(m_validationObject);

    if (tableName().isEmpty()) {
        qWarning() << tr("IqOrm: During the verification of the ORM model revealed that not set tableName.");

        result = false;
    }

    foreach (const IqOrmPropertyDescription *propDescription, m_propertyDescriptions)
    {
        Q_CHECK_PTR(propDescription);
        if (propDescription->propertyName().isEmpty())
        {
            qWarning() << tr("IqOrm: During the verification of the ORM model revealed that for "
                             "the description of the property name is not specified properties.");
            result = false;
        }
        else if (!checkPropertyDescriptionIsValid(propDescription))
        {
            qWarning() << tr("IqOrm: During the verification of the ORM model revealed that the object "
                             "has no properties \"%0\".")
                          .arg(propDescription->propertyName());
            result = false;
        }
    }

    return result;
}

/*!
 * \property IqOrmMetaModel::tableName
 *
 * Даное свойство содержит имя таблицы источника данных, в которой будут хранится прямые (простые) свойства объекта.
 *
 * IqOrm использует шаблон Active Record, поэтому для каждого класса объектов ORM используется своя таблица в источнике
 * данных, для сохранения значений объектов. Возможно хранить несколько классов объектов ORM в одной таблице, если
 * эти классы наследуются друг от друга, но в этом случае, необходимо четко понимать, что Вы делаете.
 */
QString IqOrmMetaModel::tableName() const
{
    return m_tableName;
}

void IqOrmMetaModel::setNotNull(const QString &property, bool notNull)
{
    IqOrmPropertyDescription *propDescr = const_cast<IqOrmPropertyDescription *>(propertyDescription(property));
    Q_CHECK_PTR(propDescr);
    Q_ASSERT_X(propDescr->mappedType() == IqOrmPropertyDescription::OneToOne ||
               propDescr->mappedType() == IqOrmPropertyDescription::ManyToOne,
               Q_FUNC_INFO,
               tr("Not null only supported one-to-one and many-to-one.").toLocal8Bit().constData());

    propDescr->setNotNull(notNull);
}

void IqOrmMetaModel::setReadOnly(const QString &property, bool readOnly)
{
    IqOrmPropertyDescription *propDescr = const_cast<IqOrmPropertyDescription *>(propertyDescription(property));
    Q_CHECK_PTR(propDescr);

    propDescr->setReadOnly(readOnly);
}

QStringList IqOrmMetaModel::sourcesIdList() const
{
    return QStringList() << tableName();
}
