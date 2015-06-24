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

#include "iqormobject.h"
#include "iqormcore.h"
#include "iqormerror.h"
#include "iqormabstractdatasource.h"
#include "iqormpropertydescription.h"
#include "iqormabstractobjectdatasource.h"
#include "iqormmetamodel.h"
#include "iqormabstracttriggers.h"
#include "iqormobjectrawdata.h"
#include "iqormdatasourceoperationplan.h"
#include <QPointer>
#include <QDebug>

/*!
 * \class IqOrmObject
 * \inmodule IqOrm
 *
 * \brief Класс IqOrmObject является базовым классом для работы с объектами ORM. Все объекты ORM должены быть
 * отнаследован от данного класса.
 *
 * Любой ORM объект IqOrm - это обычный QObject, с сигналами, слотами и свойствами, доступными QObject. IqOrm использует
 * свойства объекта (описнанные в Q_PROPERTY) как поля объекта ORM. Данные из свойств объекта будут сохранены в источнике
 * данных.
 *
 * Для сохранения объекта ORM в источнике данных используется шаблон проектирования Active Record, т.е. каждый объект -
 * это уникальная строка данных в источнике данных. Для иденификации объектов, у каждого из них есть свойство objectId,
 * которое будет содержать уникальный идентификатор объекта в источнике данных. Данный номер выдается объекту при его
 * первом сохранении в источник данных при помощи метода save(), и сохраняется за объектом на протяжении всего его существования,
 * пока он не будет удален из источника данных при помощи метода remove(). Зная уникальный objectId можно получить объект
 * из источника данных при помощи метода load().
 *
 * \section1 Загрузка объектов из источника данных (Load object)
 *
 * Для загрузки объекта ORM из источника данных, необходимо знать его objectId. Для загрузки объекта, необходимо использовать
 * метод load(), который принимает первым параметром objectId.
 *
 * Если необходимо провести поиск объекта в источнике данных, не зная его objectId, нужно использовать IqOrmModel.
 *
 * \section1 Сохранение объетка в источнике данных (Save object)
 *
 * Для сохранения объекта ORM в источнике данных, нужно использовать метод save(). Если объет не был загружен из источника
 * данных, то он будет сохранен в него как новый объект, и после сохранения ему будет присвоен objectId. Если же объект
 * был загружен из источника данных, то после вызова метода save(), произойдет обновление данных объекта в источнике данных,
 * при этом objectId объекта не изменится.
 *
 * \section1 Удаление объекта из источника данных (Remove object)
 *
 * Для удаления объекта ORM из источника данных, нужно использовать метод remove(). Объект для удаления должен быть
 * обезательно перед удалением либо загружен из источника данных, либо сохранен в него, иначе удаление не произойдет.
 *
 * \section1 Работа с транзакциями (Transactions)
 *
 * Каждая операция с объектом, по-умолчанию, выполняется в отдельной транзакции. Если необходимо выполнить несколько операций в
 * одной транзакции, необходимо сначала вручную открыть транзакцию, а потом использовать ее в метода load(), save(), remove().
 * Подробнее о работе с транзакциями смотри в \l {work with transaction}.
 *
 * \section1 Наследование (Inheritance)
 *
 * Для работы с объектами ORM необходимо описать эти объекты. Для этого необходимо создать класс, который будет наследован
 * от QObject (не обязательно непосредственно от QObject, но обязательно, чтоб QObject был одним из предков объекта) и
 * от IqOrmObject. К томуже необходимо в описание класса добавить макрос IQORM_OBJECT, наподобии того, как добавляется
 * макрос Q_OBJECT для объектов Qt.
 *
 * Так же необходимо зарегестрировать класс, для работы с IqOrm при помощи макроса IQORM_REGISTER_CLASS после описания класса.
 *
 * Так же в классе необходимо переопределить виртуальный метод initializeOrmMetaModel().
 *
 * Метод initializeOrmMetaModel() используется для инициализация мета-модели, связанной с данным объектом. Мета-модель используется для описания способа
 * хранения объекта в источнике данных.
 *
 * \e person.h
 *
 * \quotefile IqOrmObject/person.h
 *
 * \e person.cpp
 *
 * \quotefile IqOrmObject/person.cpp
 */

/*!
 * \macro IQORM_OBJECT
 * \relates IqOrmObject
 *
 * Макрос, который должен быть добавлен в private секцию любого класса, унаследованого от IqOrmObject, наподобии
 * макроса Q_OBJECT, для добавление необходимых свойст и методов для работы IqOrm.
 *
 * \snippet IqOrmObject/person.h IQORM_OBJECT_1
 * \dots
 * \snippet IqOrmObject/person.h IQORM_OBJECT_2
 * \dots
 * \snippet IqOrmObject/person.h IQORM_OBJECT_3
 */

/*!
 * \macro IQORM_REGISTER_CLASS(CLASS_NAME)
 * \relates IqOrmObject
 *
 * Макрос, который должен быть добавлен после описания любого класса, унаследованного от IqOrmObject, для того, чтобы
 * данный класс был зарегистрирован в IqOrm. В качестве \a CLASS_NAME необходимо передать имя класса объекта ORM.
 *
 * \snippet IqOrmObject/person.h IQORM_REGISTER_CLASS_1
 * \dots
 * \snippet IqOrmObject/person.h IQORM_REGISTER_CLASS_2
 */

/*!
 * \fn void IqOrmObject::initializeOrmMetaModel(IqOrmMetaModel *model) const
 *
 * Инициализирует ORM мета-модель, переданую в параметре \a model. Мето-модель используется для описания способа хранения
 * объекта в источнике данных. Данный метод должен быть обязательно переопределен в наследниках IqOrmObject.
 *
 * Для того, чтобы IqOrm понимала как сохранить объект ORM в источнике данных, она использует мета-модель, свзяанную
 * с объектом. Мета-модель содержит в себе всю необходимую информацию о способе отображения объекта ORM в источник данных,
 * она инициализируется однажды при запуске приложения для каждого класса. Для разных объектов ORM одного класса используется
 * одна и таже мета-модель.
 *
 * \e person.h
 * \snippet IqOrmObject/person.h initializeOrmMetaModel_1
 * \dots
 * \snippet IqOrmObject/person.h initializeOrmMetaModel_2
 * \snippet IqOrmObject/person.h initializeOrmMetaModel_3
 * \dots
 * \snippet IqOrmObject/person.h IQORM_REGISTER_CLASS_2
 *
 * \e person.cpp
 * \snippet IqOrmObject/person.cpp initializeOrmMetaModel_1
 *
 * \sa ormMetaModel()
 */

/*!
 * Создает IqOrmObject
 */
IqOrmObject::IqOrmObject():
    m_objectId(-1),
    m_dataSource(Q_NULLPTR),
    m_lastError(new IqOrmError(dynamic_cast<QObject *>(this))),
    m_isLoadedFromDataSource(false)
{    
}

IqOrmObject::~IqOrmObject()
{
}

/*!
 * \fn IqOrmObject::staticOrmMetaModel()
 *
 * Возвращает статическую мето-модель, используемую объектом
 *
 * Статическая мето-модель, используемая объектом ORM одина для всех объектов одного класса. Она заполняется один раз,
 * при старте приложения в методе initializeOrmMetaModel().
 *
 * \sa initializeOrmMetaModel(), ormMetaModel()
 */

/*!
 * Возвращает мета-модель, используемую объектом ORM. Мето-модель описывает способ хранения объекта в источнике данных.
 *
 * Если данный метод, не переопределен пользователем, то вернет статическую мето-модель, используемую всеми объектами
 * этого класса.
 *
 * \sa initializeOrmMetaModel(), staticOrmMetaModel()
 */
const IqOrmMetaModel *IqOrmObject::ormMetaModel() const
{
    return Q_NULLPTR;
}

/*!
 * \property IqOrmObject::objectId
 *
 * Данное свойство содержит уникальный идентификатор объекта ORM, для идентификации объекта в источнике данных.
 *
 * Для хранения объектов в источнике данных используется шаблон Active Record, который требует, чтоб для каждого объекта
 * в источнике данных была отдельная запись. По objectId можно однозначно идентифицировать запись в источнике данных.
 * Данное свойство нельзя установить вручную, оно берется из источника данных и устанавливается объекту при вызове методов
 * load(), save() или remove().
 *
 * По-умолчанию, \c false.
 */

qint64 IqOrmObject::objectId() const
{
    return m_objectId;
}

/*!
 * \property IqOrmObject::isPersisted
 *
 * Данное свойство содержит признак того, что объект ORM существует в источнике данных. Объект ORM существует в источнике
 * данных, если об был туда сохранен или от туда загружен.
 *
 * \sa load(), save()
 */

bool IqOrmObject::isPersisted() const
{
    return isLoadedFromDataSource() && isSavedToDataSource();
}

/*!
 * \property IqOrmObject::dataSource
 *
 * Данное свойство содержит специальный источник данных, используемый объектом.
 *
 * По-умолчанию, объекты ORM используют один источник данных, указанный в IqOrmCore::dataSource(). Но можно для определенных
 * объекто указать специальный источник данных, которые они должны использовать. В этом случае объект будет работать
 * со специальным источником данных. Данная возможность позволяет одновременно использовать объекты ORM, полученные из
 * различных источников данных.
 *
 * Если установить данное свойство в Q_NULLPTR, то будет использоваться источник данных, указанный в IqOrmCore::dataSource().
 *
 * По-умолчанию, Q_NULLPTR.
 */

IqOrmAbstractDataSource *IqOrmObject::dataSource() const
{
    return m_dataSource;
}

/*!
 * Загружает объект ORM с указанным \a objectId в транзакции \a transaction из источника данных. Если в качестве транзакции
 * передана пустая транзакция, то загрузка будет происходить в отдельной транзакции.
 *
 * Перед загрузкой и после загрузки объекта ORM, будут вызваны необходимые трегеры (см. triggers()).
 *
 * Возвращает результат загрузки. Если произошла ошибка во время загрузки, то дополнительную информацию об ошибки можно
 * получить из lastError.
 *
 * \snippet IqOrmObject/use_example.cpp load
 */
bool IqOrmObject::load(const qint64 objectId, IqOrmTransactionControl transaction)
{
    m_lastError->clearError();

    IqOrmAbstractDataSource *ds = usedDataSource();
    Q_CHECK_PTR(ds);

    if (transaction.isValid()
            && transaction.dataSource() != ds) {
        m_lastError->setText(QObject::tr("The transaction is open for another data source."));
        return false;
    }

    IqOrmTransactionControl transactionControl = transaction;
    if (!transactionControl.isValid())
        transactionControl = ds->transaction();

    if (objectId < 0) {
        m_lastError->setText(QObject::tr("ObjectId must be > -1"));
        return false;
    }

    if (!ormMetaModel()) {
        m_lastError->setText(QObject::tr("Not found valid object ORM model."));
        return false;
    }

    IqOrmDataSourceOperationPlan operationPlan;
    operationPlan.setOperation(IqOrmAbstractDataSource::Load);
    operationPlan.setObjectId(objectId);
    operationPlan.setDataSource(ds);

    QString error;
    if (triggers() && !triggers()->preLoad(this, transactionControl, operationPlan, &error)) {
        m_lastError->setText(QObject::tr("Load aborted by preLoad() trigger with error: \"%0\"")
                             .arg(error));
        return false;
    }

    IqOrmDataSourceOperationResult result = ds->objectDataSource()->loadObjectById(this, objectId);

    if (result) {
        //Сохраним значения источника
        updateSourcePropertyValues();
    } else {
        //Сбросим значения источника
        clearSourcePropertyValues();
    }

    if (result) {
        if (triggers()
                && !triggers()->postLoad(this, transactionControl, result, &error)) {
            m_lastError->setText(QObject::tr("Load aborted by postLoad() trigger with error: \"%0\"")
                                 .arg(error));
            return false;
        }

        if (!transaction.isValid())
            transactionControl.commit();
    } else {
        m_lastError->setText(result.error());
    }

    return result;
}

/*!
 * \overload load()
 *
 * Повторно загружает объект ORM из источника данных в указанной транзакции \a transaction аналогично функции reload(). Если в качестве транзакции
 * передана пустая транзакция, то загрузка будет происходить в отдельной транзакции.
 *
 * Для использования повторной загрузки объект должен быть обязательно \l {load} {загружен с использованием идентфикатора объекта}.
 *
 * Возвращает результат загрузки. Если произошла ошибка во время загрузки, то дополнительную информацию об ошибки можно
 * получить из lastError.
 */
bool IqOrmObject::load(IqOrmTransactionControl transaction)
{
    return reload(transaction);
}

/*!
 * Повторно загружает объект ORM из источника данных в указанной транзакции \a transaction. Если в качестве транзакции
 * передана пустая транзакция, то загрузка будет происходить в отдельной транзакции.
 *
 * Для использования повторной загрузки объект должен быть обязательно \l {load} {загружен с использованием идентфикатора объекта}.
 *
 * Возвращает результат загрузки. Если произошла ошибка во время загрузки, то дополнительную информацию об ошибки можно
 * получить из lastError.
 */
bool IqOrmObject::reload(IqOrmTransactionControl transaction)
{
    m_lastError->clearError();

    if (objectId() == -1) {
        m_lastError->setText(QObject::tr("Can not reload object without objectId. Use load(qint64 objectId) first."));
        return false;
    }

    return load(objectId(), transaction);
}

void IqOrmObject::setDataSource(IqOrmAbstractDataSource *dataSource)
{
    if (m_dataSource != dataSource) {
        m_dataSource = dataSource;

        emitDataSourceChanged();

        //Сбросим ид объекта, т.к. в другом источнике данных объект для устновки ид должен быть загружен
        setObjectId(-1);
    }
}

/*!
 * \property IqOrmObject::lastError
 *
 * Данное свойство содержит объект, описывающий последнюю ошибку работы с источником данных.
 *
 * При выполнение методов load(), save(), remove() и т.д., может произойти ошибка работы с источником данных. Данное свойство
 * позволяет получить доступ к объекту, описывающему последнюю ошибку.
 */

const IqOrmError *IqOrmObject::lastError() const
{
    return m_lastError;
}

IqOrmAbstractDataSource *IqOrmObject::usedDataSource() const
{
    if (m_dataSource)
        return m_dataSource;

    IqOrmAbstractDataSource *coreDataSource = IqOrmCore::dataSource();
    Q_CHECK_PTR (coreDataSource);
    Q_CHECK_PTR(coreDataSource->objectDataSource());

    return coreDataSource;
}

/*!
 * Сохраняет объект ORM в источник данных в указанной транзакции \a transaction. Если в качестве транзакции
 * передана пустая транзакция, то загрузка будет происходить в отдельной транзакции.
 *
 * Если объект ORM был загружен из источника данных, используя метод load(), то, при сохранении объекта, произойдет обновлении
 * записи источника данных, в которую был сохранен объект, и свойство objectId объекта не изменится.
 *
 * \snippet IqOrmObject/use_example.cpp save
 *
 * Если объект ORM не был загружен из источника данных, используя метод load(), то, при сохранении объекта, произойдет
 * создание новой записи в истночнке данных со значениями объекта. После хоранения, объекту будет присвоен новый уникальный
 * objectId.
 *
 * \snippet IqOrmObject/use_example.cpp update
 *
 * Возвращает результат сохранения. Если произошла ошибка во время сохранения, то дополнительную информацию об ошибки можно
 * получить из lastError.
 */
bool IqOrmObject::save(IqOrmTransactionControl transaction)
{
    m_lastError->clearError();

    IqOrmAbstractDataSource *ds = usedDataSource();
    Q_CHECK_PTR (ds);

    if (transaction.isValid()
            && transaction.dataSource() != ds) {
        m_lastError->setText(QObject::tr("The transaction is open for another data source."));
        return false;
    }

    IqOrmTransactionControl transactionControl = transaction;
    if (!transactionControl.isValid())
        transactionControl = ds->transaction();

    if (!ormMetaModel()) {
        m_lastError->setText(QObject::tr("Not found valid object ORM model."));
        return false;
    }

    bool isNewObject = !isSavedToDataSource();
    bool isLoadedObject = !isNewObject && isLoadedFromDataSource();
    Q_ASSERT(isNewObject != isLoadedObject);

    if (!isNewObject && !isLoadedObject) {
        m_lastError->setText(QObject::tr("Object exist in Data Source, but was not loaded from it. Can "
                                         "not save(). Use load() or reload() first."));
    }

    IqOrmAbstractDataSource::Operation operation = isNewObject?IqOrmAbstractDataSource::Persist:IqOrmAbstractDataSource::Update;

    QSet<const IqOrmPropertyDescription *> propertiesToSave;
    switch (operation) {
    case IqOrmAbstractDataSource::Persist:
        propertiesToSave = ormMetaModel()->propertyDescriptions();
        break;
    case IqOrmAbstractDataSource::Update:
        propertiesToSave = sourcePropertyChanges();
        break;
    default:
        break;
    }

    IqOrmDataSourceOperationPlan operationPlan;
    operationPlan.setOperation(operation);
    operationPlan.setObjectId(objectId());
    operationPlan.setDataSource(ds);
    operationPlan.setChangedProperites(propertiesToSave);

    QString error;
    if (triggers()
            && !triggers()->preSave(this, transactionControl, operationPlan, &error)) {
            m_lastError->setText(QObject::tr("Save aborted by preSave() trigger with error: \"%0\"")
                                 .arg(error));
            return false;
    }

    IqOrmDataSourceOperationResult result;

    switch (operation) {
    case IqOrmAbstractDataSource::Persist: {
        //Создаем новый объект
        result = ds->objectDataSource()->insertObject(this);

        if (result)
            updateSourcePropertyValues();

        break;
    }
    case IqOrmAbstractDataSource::Update: {
        if (propertiesToSave.isEmpty()) {
            qDebug() << QObject::tr("Call save() for an object that has no changes.");
            return true;
        }

        //Обновляем существующий объект
        result = ds->objectDataSource()->updateObject(this, propertiesToSave);

        if (result)
            updateSourcePropertyValues();

        break;
    }
    default:
        break;
    }

    if (result) {
        if (triggers()
                && !triggers()->postSave(this, transactionControl, result, &error)) {
            m_lastError->setText(QObject::tr("Save aborted by postSave() trigger with error: \"%0\"")
                                 .arg(error));
            return false;
        }

        if (!transaction.isValid())
            transactionControl.commit();
    } else {
        m_lastError->setText(result.error());
    }

    return result;
}

void IqOrmObject::setObjectId(qint64 objectId)
{
    if (m_objectId != objectId) {
        bool oldIsValid = isPersisted();
        m_objectId = objectId;

        emitObjectIdChanged();

        if (isPersisted() != oldIsValid)
            emitIsPersistedChanged();

        setIsLoadedFromDataSource(objectId != -1);
    }
}

/*!
 * Удаляет объект ORM из источника данных в указанной транзакции \a transaction.
 *
 * Удалять можно только объекты, которые храняться в источнкие данных, т.е. те объекты, которые были либо загружены из
 * источника данных методом load(), или сохранены в источник данных методом save().
 *
 * Возвращает результат удаления. Если произошла ошибка во время удаления, то дополнительную информацию об ошибки можно
 * получить из lastError.
 *
 * \note После удаления \b не происходит сброс свойств объектa ORM на значения "по-умолчанию", сбрасывается лишь свойства
 * objectId и isPersisted, т.к. объект ORM больше не хранится в источнике данных.
 *
 * \snippet IqOrmObject/use_example.cpp remove
 */
bool IqOrmObject::remove(IqOrmTransactionControl transaction)
{
    m_lastError->clearError();

    IqOrmAbstractDataSource *ds = usedDataSource();
    Q_CHECK_PTR (ds);

    if (transaction.isValid()
            && transaction.dataSource() != ds) {
        m_lastError->setText(QObject::tr("The transaction is open for another data source."));
        return false;
    }

    IqOrmTransactionControl transactionControl = transaction;
    if (!transactionControl.isValid())
        transactionControl = ds->transaction();

    if (!ormMetaModel()) {
        m_lastError->setText(QObject::tr("Not found valid object ORM model."));
        return false;
    }

    if (!isSavedToDataSource()) {
        m_lastError->setText(QObject::tr("Unable to delete an object that has not been saved in Data Source. "
                                         "Use load() or save() first."));
        return false;
    }

    if (!sourcePropertyChanges().isEmpty()) {
        qDebug() << QObject::tr("Deleted object in which a change is made. Will reload() the object.");
        if (!reload())
            return false;
    }

    IqOrmDataSourceOperationPlan operationPlan;
    operationPlan.setOperation(IqOrmAbstractDataSource::Remove);
    operationPlan.setObjectId(objectId());
    operationPlan.setDataSource(ds);

    QString error;
    if (triggers()
            && !triggers()->preRemove(this, transactionControl, operationPlan, &error)) {
        m_lastError->setText(QObject::tr("Remove aborted by preRemove() trigger with error: \"%0\"")
                             .arg(error));
        return false;
    }

    IqOrmDataSourceOperationResult result = ds->objectDataSource()->removeObject(this);

    if (result) {
        if (triggers()
                && !triggers()->postRemove(this, transactionControl, result, &error)) {
            m_lastError->setText(QObject::tr("Remove aborted by postRemove() trigger with error: \"%0\"")
                                 .arg(error));
            return false;
        }

        if (!transaction.isValid())
            transactionControl.commit();
    } else {
        m_lastError->setText(result.error());
    }

    return result;
}

void IqOrmObject::updateSourcePropertyValues()
{
    foreach (const IqOrmPropertyDescription *propDescription, ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propDescription);
        switch (propDescription->storedValue()) {
        case IqOrmPropertyDescription::SimpeVariant:
            m_sourcePropertyValues[propDescription] = propDescription->value(this);
            break;
        case IqOrmPropertyDescription::ObjectPointer: {
            const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
            m_sourcePropertyValues[propDescription] = oneObjectDescribingPropertyDescription->valueAsObjectId(this);
            break;
        }
        case IqOrmPropertyDescription::ObjectPointerList: {
            const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
            m_sourcePropertyValues[propDescription] = manyObjectDescribingPropertyDescription->valueAsObjectIds(this);
            break;
        }
        }
    }
}

void IqOrmObject::clearSourcePropertyValues()
{
    m_sourcePropertyValues.clear();
}

QSet<const IqOrmPropertyDescription *> IqOrmObject::sourcePropertyChanges() const
{
    QSet<const IqOrmPropertyDescription *> result;
    foreach (const IqOrmPropertyDescription *propDescription, ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propDescription);
        switch (propDescription->storedValue()) {
        case IqOrmPropertyDescription::SimpeVariant:
            if (m_sourcePropertyValues[propDescription] != propDescription->value(this))
                result << propDescription;
            break;
        case IqOrmPropertyDescription::ObjectPointer: {
            const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
            QVariant currentObjectId = oneObjectDescribingPropertyDescription->valueAsObjectId(this);
            QVariant sourceObjectId = m_sourcePropertyValues[propDescription];
            if(sourceObjectId != currentObjectId)
                result << propDescription;
            break;
        }
        case IqOrmPropertyDescription::ObjectPointerList: {
            const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
            QVariantList curreontObjectIds = manyObjectDescribingPropertyDescription->valueAsObjectIds(this);
            QVariantList sourceObjectIds = m_sourcePropertyValues[propDescription].toList();
            if(sourceObjectIds != curreontObjectIds)
                result << propDescription;
            break;
        }
        }
    }

    return result;
}

QVariant IqOrmObject::sourcePropertyValue(const IqOrmPropertyDescription *propertyDescription) const
{
    return m_sourcePropertyValues[const_cast<IqOrmPropertyDescription *>(propertyDescription)];
}

/*!
 * Возвращает тригеры, используемые объектом ORM.
 *
 * Тригеры позволяют выполнять определенные действия перед операциями, которые работают с источником данных, в транзакции
 * операции. Данный метод вернет тригеры, которые будут использованы для работы с объектом ORM движком IqOrm.
 *
 * Если данный метод не переопределен в наследниках, то сначала будут возвращены тригеры в следующем порядке:
 *     1. Тригеры, указаные для мета-модели объекта (см. IqOrmMetaModel::triggers());
 *     2. Тригеры, используемы по умолчанию (см. IqOrmCore::defaultTriggers()).
 *
 * Более подробно о тригерах рассказано в \l {work with triggers}.
 */

IqOrmAbstractTriggers *IqOrmObject::triggers() const
{
    Q_CHECK_PTR(ormMetaModel());

    if (ormMetaModel()->triggers())
        return ormMetaModel()->triggers();

    return IqOrmCore::defaultTriggers();
}

bool IqOrmObject::isLoadedFromDataSource() const
{
    return m_isLoadedFromDataSource;
}

void IqOrmObject::setIsLoadedFromDataSource(bool isLoadedFromDataSource)
{
    m_isLoadedFromDataSource = isLoadedFromDataSource;
}

bool IqOrmObject::isSavedToDataSource() const
{
    return objectId() != -1;
}

void IqOrmObject::setValues(const IqOrmObjectRawData &rawData)
{
    foreach (const IqOrmPropertyDescription *propDescription, ormMetaModel()->propertyDescriptions()) {
        Q_CHECK_PTR(propDescription);
        Q_ASSERT(rawData.values.contains(propDescription));

        //Устанавливаем значение
        switch (propDescription->storedValue()) {
        case IqOrmPropertyDescription::Direct: {
            propDescription->setValue(this, rawData.values[propDescription]);
            break;
        }
        case IqOrmPropertyDescription::ObjectPointer: {
            const IqOrmOneObjectDescribingPropertyDescription *oneObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(oneObjectDescribingPropertyDescription);
            bool setResult = oneObjectDescribingPropertyDescription->setValueFromObjectId(this, rawData.values[propDescription].toLongLong());
            Q_UNUSED(setResult);
            Q_ASSERT(setResult);
            break;
        }
        case IqOrmPropertyDescription::ObjectPointerList: {
            const IqOrmManyObjectDescribingPropertyDescription *manyObjectDescribingPropertyDescription
                    = dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(propDescription);
            Q_CHECK_PTR(manyObjectDescribingPropertyDescription);
            bool setResult = manyObjectDescribingPropertyDescription->setValueFromObjectIds(this, rawData.values[propDescription].toList());
            Q_UNUSED(setResult);
            Q_ASSERT(setResult);
            break;
        }
        }
    }

    setObjectId(rawData.objectId);
}
