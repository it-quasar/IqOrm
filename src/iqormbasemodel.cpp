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

#define FIRST_QML_ROLE 1000

#include "iqormbasemodel.h"
#include "iqormcore.h"
#include "iqormtransactioncontrol.h"
#include "iqormabstractdatasource.h"
#include "iqormmetamodel.h"
#include "iqormobject.h"
#include "iqormabstractmodeldatasource.h"
#include "iqormerror.h"
#include "iqormfilter.h"
#include "iqormdatasourceoperationresult.h"
#include "iqormdatasourceoperationplan.h"
#include "iqormdatasourceoperationresult.h"
#include "iqormabstracttriggers.h"
#include "iqormobjectrawdata.h"
#include "iqormtransactioncontrol.h"
#include "iqormmetamodelprivateaccessor.h"
#include <QMetaProperty>
#include <QQuickItem>
#include <QDebug>

using namespace IqOrmPrivate;

IqOrmBaseModel::IqOrmModelItem::IqOrmModelItem() :
    object(Q_NULLPTR),
    qobject(Q_NULLPTR)
{
}

IqOrmBaseModel::IqOrmBaseModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_filters(NULL),
    m_lastError(new IqOrmError(this)),
    m_dataSource(Q_NULLPTR),
    m_childChangeMonitoring(false)
{
    int indexOfOnObjectChangedMethod = this->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("onObjectChanged()").constData());
    m_onObjectChangedMethod = metaObject()->method(indexOfOnObjectChangedMethod);
}

IqOrmBaseModel::~IqOrmBaseModel()
{
    qDeleteAll(m_objectRows.keys());
    qDeleteAll(m_items);
}

QObject *IqOrmBaseModel::get(int row)
{
    if (row < 0 || row >= rowCount())
        return Q_NULLPTR;

    IqOrmModelItem *item = m_items[row];
    createItemObject(item, row);
    return item->qobject;
}

void IqOrmBaseModel::setPropertyEditable(const QString &property, const bool editable)
{
    if (editable) {
        m_editableProperties.insert(property);
    } else {
        m_editableProperties.remove(property);
    }
}

void IqOrmBaseModel::showProperty(const QString &property, const QString &headerTitle)
{
    if (m_visibleProperties.contains(property))
        return;

    emit beginInsertColumns(QModelIndex(), m_visibleProperties.count(), m_visibleProperties.count());
    m_visibleProperties.append(property);
    if (!headerTitle.isEmpty())
        m_propertyHeaderTitles[property] = headerTitle;
    else
        m_propertyHeaderTitles[property] = property;
    emit endInsertColumns();
}

void IqOrmBaseModel::hideProperty(const QString &property)
{
    if (!m_visibleProperties.contains(property))
        return;

    emit beginRemoveColumns(QModelIndex(), m_visibleProperties.indexOf(property), m_visibleProperties.indexOf(property));
    m_visibleProperties.removeOne(property);
    m_propertyHeaderTitles.remove(property);
    emit endRemoveColumns();
}

void IqOrmBaseModel::setPropertyHeader(const QString &property, const QString &headerTitle)
{
    if (m_propertyHeaderTitles[property] == headerTitle)
        return;

    m_propertyHeaderTitles[property] = headerTitle;

    if (m_visibleProperties.contains(property))
        emit headerDataChanged(Qt::Horizontal, m_visibleProperties.indexOf(property), m_visibleProperties.indexOf(property));
}

QObject *IqOrmBaseModel::first()
{
    if (rowCount() == 0)
        return Q_NULLPTR;
    return get(0);
}

QObject *IqOrmBaseModel::last()
{
    if (rowCount() == 0)
        return Q_NULLPTR;
    return get(rowCount() - 1);
}

bool IqOrmBaseModel::findAll(IqOrmTransactionControl transaction)
{
    setFilters(Q_NULLPTR);
    return load(transaction);
}

bool IqOrmBaseModel::find(const QString &propertyName, IqOrmFilter::Condition operation, const QVariant &value, IqOrmTransactionControl transaction)
{
    return find(propertyName, operation, value, Qt::CaseSensitive, transaction);
}

bool IqOrmBaseModel::find(const QString &propertyName, IqOrmFilter::Condition operation, const QVariant &value, Qt::CaseSensitivity caseSensitivity, IqOrmTransactionControl transaction)
{
    IqOrmFilter *filter = new IqOrmFilter(propertyName, operation, value, this);
    filter->setCaseSensitivity(caseSensitivity);
    setFilters(filter);
    return load(transaction);
}

bool IqOrmBaseModel::load(IqOrmTransactionControl transaction)
{
    return load(-1, 0, Asc, transaction);
}

bool IqOrmBaseModel::load(qint64 limit, qint64 offset, OrderBy orderBy, IqOrmTransactionControl transaction)
{
    lastError()->clearError();

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

    const IqOrmMetaModel *ormModel = childsOrmMetaModel();
    if (!ormModel) {
        lastError()->setText(QObject::tr("Not found valid child objects ORM model."));
        return false;
    }

    IqOrmAbstractModelDataSource::OrderBy dataSourceOrderBy = IqOrmAbstractModelDataSource::Asc;
    switch (orderBy) {
    case Asc:
        dataSourceOrderBy = IqOrmAbstractModelDataSource::Asc;
        break;
    case Desc:
        dataSourceOrderBy = IqOrmAbstractModelDataSource::Desc;
        break;
    }

    IqOrmDataSourceOperationResult result = ds->objectsModelDataSource()->loadModel(this, limit, offset, dataSourceOrderBy);

    if (result) {
        if (!transaction.isValid())
            transactionControl.commit();
    } else {
        lastError()->setText(result.error());
    }

    if (!m_childChangeMonitoring)
        emit dataChanged(index(0,0), index(rowCount() - 1, columnCount() - 1));

    return result;
}

IqOrmError *IqOrmBaseModel::lastError() const
{
    return m_lastError;
}

IqOrmAbstractFilter *IqOrmBaseModel::filters() const
{
    return m_filters;
}

void IqOrmBaseModel::onObjectChanged()
{
    QObject * senderItem = sender();
    if (!senderItem)
        return;

    if (senderSignalIndex() < 0)
        return;

    QString property = m_signalIndexProperty[senderSignalIndex()];
    if (property.isEmpty())
        return;

    IqOrmObject *senderObject = dynamic_cast<IqOrmObject *>(senderItem);
    Q_CHECK_PTR(senderObject);
    Q_ASSERT(m_objectRows.contains(senderObject));

    int itemRow = m_objectRows[senderObject];
    int propertyColumn = m_visibleProperties.indexOf(property);

    if (propertyColumn != -1) {
        QModelIndex topLeft = index(itemRow, propertyColumn);
        QModelIndex bottomRigth = index(itemRow, propertyColumn);
        emit dataChanged(topLeft, bottomRigth);
    } else {
        QModelIndex topLeft = index(itemRow, 0);
        QModelIndex bottomRigth = index(itemRow, 0);
        emit dataChanged(topLeft, bottomRigth);

        emit invisibleDataChanged(itemRow, property);
    }
}

void IqOrmBaseModel::setFilters(IqOrmAbstractFilter *filters)
{
    if (m_filters != filters) {
        if (m_filters)
            m_filters->deleteLater();

        m_filters = filters;
        m_filters->setParent(this);
        emit filtersChanged();
    }
}

bool IqOrmBaseModel::loadFirst(qint64 count, IqOrmTransactionControl transaction)
{
    return load(count, 0, Asc, transaction);
}

bool IqOrmBaseModel::loadLast(qint64 count, IqOrmTransactionControl transaction)
{
    return load(count, 0, Desc, transaction);
}

bool IqOrmBaseModel::truncate(IqOrmTransactionControl transaction)
{
    lastError()->clearError();

    const IqOrmMetaModel *ormModel = childsOrmMetaModel();
    if (!ormModel) {
        lastError()->setText(QObject::tr("Not found valid child objects ORM model."));
        return false;
    }

    QString error;
    bool result = processTruncate(ormModel, transaction, usedDataSource(), &error);

    if (result)
        clear();
    else
        lastError()->setText(error);

    return result;
}

qint64 IqOrmBaseModel::loadCount(IqOrmTransactionControl transaction) const
{
    lastError()->clearError();

    IqOrmAbstractDataSource *ds = usedDataSource();
    Q_CHECK_PTR(ds);

    if (transaction.isValid()
            && transaction.dataSource() != ds) {
        m_lastError->setText(QObject::tr("The transaction is open for another data source."));
        return -1;
    }

    IqOrmTransactionControl transactionControl = transaction;
    if (!transactionControl.isValid())
        transactionControl = ds->transaction();

    const IqOrmMetaModel *ormModel = childsOrmMetaModel();
    if (!ormModel) {
        lastError()->setText(QObject::tr("Not found valid child objects ORM model."));
        return -1;
    }

    qint64 countResult;
    IqOrmDataSourceOperationResult result = ds->objectsModelDataSource()->loadCount(this, &countResult);

    if (result) {
        if (!transaction.isValid())
            transactionControl.commit();
    } else {
        lastError()->setText(result.error());
    }

    return countResult;
}

QList<IqOrmObject *> IqOrmBaseModel::toObjectList()
{
    QList<IqOrmObject *> result;
    for (int i = 0; i < rowCount(); ++i){
        IqOrmModelItem *item = m_items[i];
        createItemObject(item, i);
        result << item->object;
    }
    return result;
}

qint64 IqOrmBaseModel::count() const
{
    return rowCount();
}

bool IqOrmBaseModel::isEmpty() const
{
    return rowCount() == 0;
}

void IqOrmBaseModel::clear()
{
    if (rowCount() == 0)
        return;

    emit beginRemoveRows(QModelIndex(), 0, rowCount() - 1);

    foreach (IqOrmModelItem *item, m_items) {
        if (item->qobject)
            item->qobject->deleteLater();
    }
    qDeleteAll(m_items);
    m_items.clear();
    m_objectRows.clear();

    emit endRemoveRows();
    emit countChanged();
}


void IqOrmBaseModel::insertItems(qint64 row, QList<IqOrmModelItem *> items)
{
    int rowToInsert = row < 0?0:row;

    if (rowToInsert > rowCount())
        rowToInsert = rowCount();

    emit beginInsertRows(QModelIndex(), rowToInsert, rowToInsert + items.count() - 1);

    int j = -1;
    for (int i = rowToInsert; i < rowToInsert + items.count(); ++i) {
        ++j;
        IqOrmModelItem *item = items[j];
        Q_CHECK_PTR(item);

        m_items.insert(i, item);
    }

    emit endInsertRows();
    emit countChanged();
}

void IqOrmBaseModel::setObjectsValues(const QList<IqOrmObjectRawData> &objectValues)
{
    //Список объектов на удаление
    QList<IqOrmModelItem *> objectsToRemove = m_items;
    //Объекты на удаление
    QList<IqOrmModelItem *> objectsToAdd;

    //Для быстрого поиска сделаем список ид объектов
    QHash<IqOrmModelItem *, qint64> objectRows;
    QHash<qint64, qint64> objectIdsRows;
    for (int i = 0; i < objectsToRemove.count(); ++i) {
        IqOrmModelItem *item = objectsToRemove[i];
        Q_CHECK_PTR(item);
        objectRows[item] = i;
        if (item->object)
            objectIdsRows[item->object->objectId()] = i;
        else
            objectIdsRows[item->rawData.objectId] = i;
    }

    //Пройдемся по всем записям
    foreach (const IqOrmObjectRawData &rawData, objectValues) {
        //Получим objectId
        qint64 objectId = rawData.objectId;

        //Если модель содержит объект с данным ид
        if (objectIdsRows.contains(objectId)) {
            objectsToAdd << Q_NULLPTR;

            //Найдем объект с таким ид
            IqOrmModelItem *item = m_items[objectIdsRows[objectId]];
            Q_CHECK_PTR(item);

            //Установим для объекта параметры из запроса
            if (item->object) {
                IqOrmObjectPrivateAccessor::setVaules(item->object, rawData);
                IqOrmObjectPrivateAccessor::updateObjectSourceProperites(item->object);
            } else {
                item->rawData = rawData;
            }
            //Такой объект есть в модели и в выборке, значит удалять его не надо
            objectsToRemove.removeOne(item);
        } else {
            //Если не нашли объект с таким ид, то создадим новый объект
            IqOrmModelItem *newItem = new IqOrmModelItem();
            newItem->rawData = rawData;

            //ДОбавим объект для сохранения
            objectsToAdd << newItem;
        }
    }

    //Удалим из модели все объекты на удаление
    //Удалять будем пачками
    IqOrmModelItem *object = Q_NULLPTR;
    for (int i = objectsToRemove.count() - 1; i > -1; --i) {
        object = objectsToRemove[i];
        Q_CHECK_PTR(object);

        int rowCount = 1;
        int firstRow = objectRows[object];
        for (int j = i - 1; j > -1; --j) {
            object = objectsToRemove[j];
            Q_CHECK_PTR(object);

            int currentRow = objectRows[object];
            if (currentRow == firstRow - 1) {
                rowCount++;
                firstRow = currentRow;
            } else {
                break;
            }
        }

        removeRows(firstRow, rowCount);

        i = i - rowCount + 1;
    }

    //Добавим новые объекты
    //Добавлять будем пачками
    for (int i = 0; i < objectsToAdd.count(); ++i) {
        object = objectsToAdd[i];
        if (!object)
            continue;

        QList<IqOrmModelItem *> rowsToAdd;
        rowsToAdd << object;
        for (int j = i + 1; j < objectsToAdd.count(); ++j) {
            object = objectsToAdd[j];
            if (!object)
                break;
            rowsToAdd << object;
        }

        insertItems(i, rowsToAdd);

        i = i + rowsToAdd.count() - 1;
    }
}

void IqOrmBaseModel::enableChildMonitoring(IqOrmObject *child) const
{
    QObject *qobject = dynamic_cast<QObject *>(child);
    Q_CHECK_PTR(qobject);
    foreach (const IqOrmPropertyDescription *propetyDescription, IqOrmMetaModelPrivateAccessor::propertyDescriptions(childsOrmMetaModel())) {
        QMetaMethod notifiSignal = propetyDescription->targetStaticMetaPropery().notifySignal();
        if (!notifiSignal.isValid())
            continue;

        connect(qobject,
                notifiSignal,
                this,
                m_onObjectChangedMethod);
    }
}

void IqOrmBaseModel::disableChildMonitoring(IqOrmObject *child) const
{
    QObject *qobject = dynamic_cast<QObject *>(child);
    Q_CHECK_PTR(qobject);
    disconnect(qobject, 0, this, 0);
}

void IqOrmBaseModel::createItemObject(IqOrmBaseModel::IqOrmModelItem *item, qint64 row)
{
    Q_CHECK_PTR(item);
    if (!item->object) {
        IqOrmObject *itemObject = createChildObject();

        IqOrmAbstractTriggers *triggers = IqOrmObjectPrivateAccessor::triggers(itemObject);
        IqOrmAbstractDataSource *ds = usedDataSource();
        Q_CHECK_PTR (ds);
        IqOrmTransactionControl transactionControll;
        bool triggersOk = true;
        if (triggers)
            transactionControll = ds->transaction();

        if (triggers) {
            IqOrmDataSourceOperationPlan operationPlan;
            operationPlan.setOperation(IqOrmAbstractDataSource::Load);
            operationPlan.setObjectId(item->rawData.objectId);
            operationPlan.setDataSource(ds);

            QString error;
            bool result = triggers->preLoad(itemObject, transactionControll, operationPlan, &error);
            if (!result)
                qWarning("On create IqOrmModel<%s> item object pre load triggers return error: \"%s\".",
                         IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(childsOrmMetaModel())->className(),
                         error.toLocal8Bit().constData());

            triggersOk = triggersOk && result;
        }

        IqOrmObjectPrivateAccessor::setVaules(itemObject, item->rawData);
        IqOrmObjectPrivateAccessor::updateObjectSourceProperites(itemObject);

        if (triggers) {
            IqOrmDataSourceOperationResult operationResult;
            operationResult.setOperation(IqOrmAbstractDataSource::Load);
            operationResult.setObjectId(itemObject->objectId());
            operationResult.setDataSource(usedDataSource());

            QString error;
            bool result = triggers->postLoad(itemObject, transactionControll, operationResult, &error);
            if (!result)
                qWarning("On create IqOrmModel<%s> item object post load triggers return error: \"%s\".",
                         IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(childsOrmMetaModel())->className(),
                         error.toLocal8Bit().constData());

            triggersOk = triggersOk && result;
        }

        if (triggers && triggersOk)
            transactionControll.commit();

        item->object = itemObject;
        item->qobject = dynamic_cast<QObject *>(itemObject);
        if (m_childChangeMonitoring)
            enableChildMonitoring(item->object);
        item->rawData = IqOrmObjectRawData();

        m_objectRows.insert(const_cast<const IqOrmObject *>(item->object), row);
    }
}

IqOrmAbstractDataSource *IqOrmBaseModel::dataSource() const
{
    return m_dataSource;
}

void IqOrmBaseModel::setDataSource(IqOrmAbstractDataSource *dataSource)
{
    if (m_dataSource != dataSource) {
        m_dataSource = dataSource;
        emit dataSourceChanged();

        foreach (IqOrmModelItem *item, m_items) {
            if (item->object)
                item->object->setDataSource(dataSource);
        }
    }
}


bool IqOrmBaseModel::childChangeMonitoring() const
{
    return m_childChangeMonitoring;
}

void IqOrmBaseModel::setChildChangeMonitoring(bool childChangeMonitoring)
{
    m_childChangeMonitoring = childChangeMonitoring;
    if (m_childChangeMonitoring) {
        foreach (IqOrmModelItem *item, m_items) {
            if (item->object)
                enableChildMonitoring(item->object);
        }
    } else {
        foreach (IqOrmModelItem *item, m_items) {
            if (item->object)
                disableChildMonitoring(item->object);
        }
    }
}

bool IqOrmBaseModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (row < 0 || row + count > rowCount())
        return false;

    emit beginRemoveRows(parent, row, row + count - 1);

    for (int i = row + count - 1; i >= row; --i) {
        IqOrmModelItem *item = m_items[i];
        Q_CHECK_PTR(item);
        if (item->object)
            item->qobject->deleteLater();
        m_objectRows.remove(item->object);
        m_items.removeAt(i);
        delete item;
    }

    emit endRemoveRows();
    emit countChanged();

    return true;
}

IqOrmObject *IqOrmBaseModel::take(int row)
{
    Q_ASSERT(row >= 0 && row < count());
    IqOrmObject *object = dynamic_cast<IqOrmObject *>(get(row));
    Q_CHECK_PTR(object);

    disableChildMonitoring(object);

    Q_ASSERT(m_objectRows.contains(object));

    emit beginRemoveRows(QModelIndex(), row, row);

    IqOrmModelItem *item = m_items[row];
    Q_CHECK_PTR(item);
    delete item;
    m_items.removeAt(row);
    m_objectRows.remove(object);

    emit endRemoveRows();
    emit countChanged();

    return object;
}

IqOrmObject *IqOrmBaseModel::take(IqOrmObject *object)
{
    if (m_objectRows.contains(object)) {
        int objectRow = m_objectRows[object];

        return take(objectRow);
    }
    return Q_NULLPTR;
}

IqOrmObject *IqOrmBaseModel::takeFirst()
{
    return take(0);
}

QVariant IqOrmBaseModel::objectData(qint64 row, const QString &property) const
{
    Q_ASSERT(row > -1);
    Q_ASSERT(row < rowCount());

    IqOrmModelItem* item = m_items.at(row);
    Q_CHECK_PTR(item);

    Q_CHECK_PTR(childsOrmMetaModel());
    if (property == QLatin1String("objectId")) {
        if (item->object)
            return item->object->objectId();
        else
            return item->rawData.objectId;
    } else {
        const IqOrmPropertyDescription *propetyDescription = IqOrmMetaModelPrivateAccessor::propertyDescription(childsOrmMetaModel(), property);
        Q_ASSERT_X(propetyDescription,
                   Q_FUNC_INFO,
                   qPrintable(tr("Description for property \"%0\" not found in IqOrmMetaModel for class \"%1\".")
                              .arg(property)
                              .arg(IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(childsOrmMetaModel())->className())));
        if (item->object)
            return propetyDescription->value(item->object);
        else
            return item->rawData.values[propetyDescription];
    }

    return QVariant();
}

int IqOrmBaseModel::rowOf(const IqOrmObject *object) const
{
    if (!m_objectRows.contains(object))
        return -1;
    return m_objectRows[object];
}

QVariant IqOrmBaseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QAbstractTableModel::headerData(section, orientation, role);

    if (orientation == Qt::Vertical)
        return QAbstractTableModel::headerData(section, orientation, role);

    if (section >= m_visibleProperties.count() || section < 0)
        return QAbstractTableModel::headerData(section, orientation, role);

    QString property = m_visibleProperties[section];

    if (role == Qt::DisplayRole)
        return m_propertyHeaderTitles[property];

    return QAbstractTableModel::headerData(section, orientation, role);
}

Qt::ItemFlags IqOrmBaseModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags result = QAbstractTableModel::flags(index);

    if (index.column() < 0 || index.column() >= columnCount())
        return result;

    if (index.row() < 0 || index.row() >= rowCount())
        return result;

    QString propertyName = m_visibleProperties[index.column()];

    Q_CHECK_PTR(childsOrmMetaModel());
    Q_CHECK_PTR(IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(childsOrmMetaModel()));

    int propertyIndex = IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(childsOrmMetaModel())->indexOfProperty(propertyName.toLocal8Bit().constData());
    QMetaProperty property = IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(childsOrmMetaModel())->property(propertyIndex);

    if (m_editableProperties.contains(propertyName)
            && property.isWritable())
        result = result | Qt::ItemIsEditable;

    return result;
}

QHash<int, QByteArray> IqOrmBaseModel::roleNames() const
{
    QHash<int, QByteArray> result;
    result[FIRST_QML_ROLE] = "objectId";

    Q_CHECK_PTR(childsOrmMetaModel());
    int i = FIRST_QML_ROLE + 1;
    foreach (const IqOrmPropertyDescription *propertyDescription, IqOrmMetaModelPrivateAccessor::propertyDescriptions(childsOrmMetaModel())) {
        result[i] = propertyDescription->propertyName().toLocal8Bit().constData();
        ++i;
    }

    return result;
}

int IqOrmBaseModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_items.count();
}

int IqOrmBaseModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    //Хак для правильного отображения в QML
    //Если модель создана в QML, то всегда должна быть как минимут одна колонка
    if(dynamic_cast<QQuickItem *>(QObject::parent()))
        return m_visibleProperties.count() > 0?m_visibleProperties.count():1;

    return m_visibleProperties.count();
}

bool IqOrmBaseModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent);
    Q_UNUSED(destinationParent);
    if (sourceRow < 0 || sourceRow >= rowCount())
        return false;

    int correctedCount = m_items.count() - (sourceRow + count) < 0 ? m_items.count() - sourceRow : count;

    int correctedDestinationChild = destinationChild;

    if (destinationChild < 0)
        correctedDestinationChild = 0;

    if (destinationChild + correctedCount > m_items.count() + 1)
        correctedDestinationChild = m_items.count() - correctedCount;

    if (sourceRow == correctedDestinationChild)
        return false;

    emit beginMoveRows(sourceParent, sourceRow, sourceRow + correctedCount - 1, destinationParent, correctedDestinationChild);

    QList<IqOrmModelItem *> temp;

    for (int i = sourceRow + correctedCount - 1; i >= sourceRow ; i--) {
        temp << m_items[i];
        m_items.removeAt(i);
    }

    for (int i = 0; i < correctedCount; i++) {
        if (destinationChild < sourceRow) {
            m_items.insert(destinationChild + i, temp[i]);
        } else {
            m_items.insert(destinationChild + i - 1, temp[i]);
        }
    }

    emit endMoveRows();

    return true;
}

QVariant IqOrmBaseModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole
            && role != Qt::EditRole
            && role < FIRST_QML_ROLE)
        return QVariant();

    int row = index.row();
    int column = index.column();

    Q_ASSERT(row > -1);
    Q_ASSERT(row < rowCount());
    Q_ASSERT(column > -1);
    Q_ASSERT(column < columnCount());

    QHash<int, QByteArray> qmlRoleNames = roleNames();

    Q_ASSERT(role < FIRST_QML_ROLE + qmlRoleNames.count());

    IqOrmModelItem* item = m_items.at(row);
    Q_CHECK_PTR(item);

    QString propertyName = "";
    if (role < FIRST_QML_ROLE) {
        Q_ASSERT(column < m_visibleProperties.count());
        propertyName = m_visibleProperties[column];
    } else {
        propertyName = qmlRoleNames.value(role);
    }

    Q_ASSERT(!propertyName.isEmpty());

    return objectData(row, propertyName);
}

bool IqOrmBaseModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    if (row < 0 || row >= rowCount())
        return false;

    int column = index.column();
    if (column < 0 || column >= columnCount())
        return false;

    QHash<int, QByteArray> qmlRoleNames = roleNames();

    if (role >= FIRST_QML_ROLE + qmlRoleNames.count())
        return false;

    IqOrmObject* object = dynamic_cast<IqOrmObject *>(get(row));
    Q_CHECK_PTR(object);

    QString propertyName = "";
    if (role < FIRST_QML_ROLE) {
        if (column < m_visibleProperties.count())
            propertyName = m_visibleProperties[column];
    } else {
        propertyName = qmlRoleNames.value(role);
    }

    if (propertyName.isEmpty())
        return false;

    Q_CHECK_PTR(childsOrmMetaModel());
    const IqOrmPropertyDescription *propetyDescription = IqOrmMetaModelPrivateAccessor::propertyDescription(childsOrmMetaModel(), propertyName);
    Q_CHECK_PTR(propetyDescription);

    bool result = propetyDescription->setValue(object, value);

    if (result)
        emit dataChanged(this->index(row,column), this->index(row, column));

    return result;
}

void IqOrmBaseModel::addPropertySiagnalIndex(const QString &property, int signalIndex)
{
    m_signalIndexProperty[signalIndex] = property;
}

bool IqOrmBaseModel::processTruncate(const IqOrmMetaModel *childOrmModel,
                                     IqOrmTransactionControl transaction,
                                     IqOrmAbstractDataSource *dataSource,
                                     QString *error)
{
    Q_CHECK_PTR(dataSource);

    if (transaction.isValid()
            && transaction.dataSource() != dataSource) {
        if (error)
            *error = tr("The transaction is open for another data source.");
        return false;
    }

    IqOrmTransactionControl transactionControl = transaction;
    if (!transactionControl.isValid())
        transactionControl = dataSource->transaction();

    IqOrmDataSourceOperationResult result = dataSource->objectsModelDataSource()->truncateModel(childOrmModel);

    if (result) {
        if (!transaction.isValid())
            transactionControl.commit();
    } else {
        if (error)
            *error = result.error();
    }

    return result;
}

IqOrmAbstractDataSource *IqOrmBaseModel::usedDataSource() const
{
    if (m_dataSource)
        return m_dataSource;

    IqOrmAbstractDataSource *coreDataSource = IqOrmCore::dataSource();
    Q_CHECK_PTR (coreDataSource);
    Q_CHECK_PTR(coreDataSource->objectDataSource());

    return coreDataSource;
}
