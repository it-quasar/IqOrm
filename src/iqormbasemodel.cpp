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
#include <QMetaProperty>
#include <QQuickItem>

IqOrmBaseModel::IqOrmBaseModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_indexOfOnObjectChangedMethod(-1),
    m_filters(NULL),
    m_lastError(new IqOrmError(this)),
    m_lastDataSource(Q_NULLPTR)
{
    m_indexOfOnObjectChangedMethod = this->metaObject()->indexOfMethod(QMetaObject::normalizedSignature("onObjectChanged()").constData());
}

IqOrmBaseModel::~IqOrmBaseModel()
{
}

QObject *IqOrmBaseModel::get(int row) const
{
    if (row < 0 || row >= rowCount())
        return Q_NULLPTR;

    return dynamic_cast<QObject *>(m_items.at(row));
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

QObject *IqOrmBaseModel::first() const
{
    if (rowCount() == 0)
        return Q_NULLPTR;
    return get(0);
}

QObject *IqOrmBaseModel::last() const
{
    if (rowCount() == 0)
        return Q_NULLPTR;
    return get(rowCount() - 1);
}

bool IqOrmBaseModel::find(const QString &propertyName, IqOrmFilter::Operation operation, const QVariant &value, IqOrmAbstractDataSource *dataSource)
{
    return find(propertyName, operation, value, Qt::CaseSensitive, dataSource);
}

bool IqOrmBaseModel::find(const QString &propertyName, IqOrmFilter::Operation operation, const QVariant &value, Qt::CaseSensitivity caseSensitivity, IqOrmAbstractDataSource *dataSource)
{
    IqOrmFilter *filter = new IqOrmFilter(propertyName, operation, value, this);
    filter->setCaseSensitivity(caseSensitivity);
    setFilters(filter);
    return load(dataSource);
}

bool IqOrmBaseModel::load(IqOrmAbstractDataSource *dataSource)
{
    return load(-1, 0, Asc, dataSource);
}

bool IqOrmBaseModel::load(qint64 limit, qint64 offset, IqOrmBaseModel::OrderBy orderBy, IqOrmAbstractDataSource *dataSource)
{
    lastError()->clearError();

    if (dataSource) {
        if (m_lastDataSource.data() != dataSource)
            m_lastDataSource = dataSource;
    }

    if (!m_lastDataSource)
        m_lastDataSource = IqOrmCore::dataSource();

    if (!m_lastDataSource) {
        lastError()->setText(QObject::tr("Not found any Data Source. Use IQORMCore::setDataSource() first."));
        return false;
    }

    if (!m_lastDataSource->objectsModelDataSource()) {
        lastError()->setText(QObject::tr("Not found any Objects Model Data Source. Maybe Data Source broken, check Data Source."));
        return false;
    }

    const IqOrmMetaModel *ormModel = childsOrmMetaModel();
    if (!ormModel) {
        lastError()->setText(QObject::tr("Not found valid child objects ORM model."));
        return false;
    }

    IqOrmAbstractModelDataSource::OrderBy dataSourceOrderBy;
    switch (orderBy) {
    case Asc:
        dataSourceOrderBy = IqOrmAbstractModelDataSource::Asc;
        break;
    case Desc:
        dataSourceOrderBy = IqOrmAbstractModelDataSource::Desc;
        break;

    }

    IqOrmTransactionControl transactionControl = m_lastDataSource->transaction();

    IqOrmDataSourceOperationResult result = m_lastDataSource->objectsModelDataSource()->loadModel(this, limit, offset, dataSourceOrderBy);

    if (result) {
        transactionControl.commit();
    } else {
        lastError()->setText(result.error());
    }

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
    Q_ASSERT(m_items.contains(senderObject));

    int itemRow = m_items.indexOf(senderObject);
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

bool IqOrmBaseModel::loadFirst(qint64 count, IqOrmAbstractDataSource *dataSource)
{
    return load(count, 0, Asc, dataSource);
}

bool IqOrmBaseModel::loadLast(qint64 count, IqOrmAbstractDataSource *dataSource)
{
    return load(count, 0, Desc, dataSource);
}

bool IqOrmBaseModel::truncate(IqOrmAbstractDataSource *dataSource)
{
    lastError()->clearError();

    if (dataSource) {
        if (m_lastDataSource.data() != dataSource)
            m_lastDataSource = dataSource;
    }

    if (!m_lastDataSource)
        m_lastDataSource = IqOrmCore::dataSource();

    QString error;
    bool result = processTruncate(childsOrmMetaModel(), m_lastDataSource, &error);

    if (result)
        clear();
    else
        lastError()->setText(error);

    return result;
}

QList<IqOrmObject *> IqOrmBaseModel::toObjectList() const
{
    return m_items;
}

qint64 IqOrmBaseModel::count() const
{
    return rowCount();
}

void IqOrmBaseModel::clear()
{
    if (rowCount() == 0)
        return;

    emit beginRemoveRows(QModelIndex(), 0, rowCount() - 1);

    for (int i = 0; i < rowCount(); ++i) {
        QObject *item = get(i);
        if (item->parent() == this)
            item->deleteLater();
    }

    m_items.clear();

    emit endRemoveRows();
    emit countChanged();
}

void IqOrmBaseModel::append(IqOrmObject *object)
{
    insert(rowCount(), object);
}

void IqOrmBaseModel::insert(int row, IqOrmObject *object)
{
    if (m_items.contains(object))
        return;

    int rowToInsert = row < 0?0:row;

    if (rowToInsert > rowCount())
        rowToInsert = rowCount();

    emit beginInsertRows(QModelIndex(), rowToInsert, rowToInsert);

    m_items.insert(rowToInsert, object);

    QObject *qobject = dynamic_cast<QObject *>(object);
    Q_CHECK_PTR(qobject);
    qobject->setParent(this);

    Q_CHECK_PTR(childsOrmMetaModel());
    Q_ASSERT_X(qobject->metaObject()->className() == childsOrmMetaModel()->targetStaticMetaObject()->className(),
               Q_FUNC_INFO,
               "Try to insert object with diferent type.");

    foreach (const IqOrmPropertyDescription *propetyDescription, childsOrmMetaModel()->propertyDescriptions()) {
        QMetaMethod notifiSignal = propetyDescription->targetStaticMetaPropery().notifySignal();
        if (!notifiSignal.isValid())
            continue;

        connect(qobject,
                notifiSignal,
                this,
                metaObject()->method(m_indexOfOnObjectChangedMethod));
    }

    emit endInsertRows();
    emit countChanged();
}

void IqOrmBaseModel::remove(IqOrmObject *object)
{
    if (!m_items.contains(object))
        return;

    qint64 oldCount = count();

    emit beginRemoveRows(QModelIndex(), m_items.indexOf(object), m_items.indexOf(object));

    Q_ASSERT(m_items.removeOne(object));
    QObject *qobject = dynamic_cast<QObject *>(object);
    Q_CHECK_PTR(qobject);
    disconnect(qobject, 0, this, 0);
    if (qobject->parent() == this)
        qobject->deleteLater();

    emit endRemoveRows();

    if (count() != oldCount)
        emit countChanged();
}

IqOrmObject *IqOrmBaseModel::take(int row)
{
    Q_ASSERT(row >= 0 && row < count());
    IqOrmObject *object = m_items[row];
    Q_CHECK_PTR(object);

    QObject *qobject = dynamic_cast<QObject *>(object);
    Q_CHECK_PTR(qobject);
    qobject->setParent(Q_NULLPTR);

    remove(object);

    return object;
}

IqOrmObject *IqOrmBaseModel::take(IqOrmObject *object)
{
    int objectRow = m_items.indexOf(object);
    if (objectRow == -1)
        return Q_NULLPTR;

    return take(objectRow);
}

int IqOrmBaseModel::rowOf(const IqOrmObject *object) const
{
    return m_items.indexOf(const_cast<IqOrmObject *>(object));
}

IqOrmAbstractDataSource *IqOrmBaseModel::lastDataSource() const
{
    return m_lastDataSource;
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
    Q_CHECK_PTR(childsOrmMetaModel()->targetStaticMetaObject());

    int propertyIndex = childsOrmMetaModel()->targetStaticMetaObject()->indexOfProperty(propertyName.toLocal8Bit().constData());
    QMetaProperty property = childsOrmMetaModel()->targetStaticMetaObject()->property(propertyIndex);

    if (m_editableProperties.contains(propertyName)
            && property.isWritable())
        result = result | Qt::ItemIsEditable;

    return result;
}

QHash<int, QByteArray> IqOrmBaseModel::roleNames() const
{
    QHash<int, QByteArray> result;

    Q_CHECK_PTR(childsOrmMetaModel());
    int i = FIRST_QML_ROLE;
    foreach (const IqOrmPropertyDescription *propertyDescription, childsOrmMetaModel()->propertyDescriptions()) {
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

    QList<IqOrmObject *> temp;

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
    int row = index.row();
    int column = index.column();

    if (row >= rowCount() || row < 0)
        return QVariant();

    if (column >= columnCount() || column < 0)
        return QVariant();

    QHash<int, QByteArray> qmlRoleNames = roleNames();

    if (role >= FIRST_QML_ROLE + qmlRoleNames.count())
        return QVariant();

    IqOrmObject* item = m_items.at(row);
    if (!item)
        return QVariant();

    QString propertyName = "";
    if (role < FIRST_QML_ROLE) {
        if (column < m_visibleProperties.count())
            propertyName = m_visibleProperties[column];
    } else {
        propertyName = qmlRoleNames.value(role);
    }

    if (propertyName.isEmpty())
        return QVariant();

    Q_CHECK_PTR(childsOrmMetaModel());
    if (role == Qt::DisplayRole
            || role == Qt::EditRole
            || role > FIRST_QML_ROLE) {
        if (propertyName == QLatin1String("objectId")) {
            return item->objectId();
        } else {
            const IqOrmPropertyDescription *propetyDescription = childsOrmMetaModel()->propertyDescription(propertyName);
            Q_ASSERT_X(propetyDescription,
                       Q_FUNC_INFO,
                       tr("Description for property \"%0\" not found in IqOrmMetaModel for class \"%1\".")
                       .arg(propertyName)
                       .arg(childsOrmMetaModel()->targetStaticMetaObject()->className()).toLocal8Bit().constData());
            return propetyDescription->value(item);
        }
    }

    return QVariant();
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

    IqOrmObject* item = m_items[row];
    if (!item)
        return false;

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
    const IqOrmPropertyDescription *propetyDescription = childsOrmMetaModel()->propertyDescription(propertyName);
    Q_CHECK_PTR(propetyDescription);

    bool result = propetyDescription->setValue(item, value);

    if (result)
        emit dataChanged(this->index(row,column), this->index(row, column));

    return result;
}

void IqOrmBaseModel::addPropertySiagnalIndex(const QString &property, int signalIndex)
{
    m_signalIndexProperty[signalIndex] = property;
}

bool IqOrmBaseModel::processTruncate(const IqOrmMetaModel *childOrmModel,
                                     IqOrmAbstractDataSource *dataSource,
                                     QString *error)
{
    IqOrmAbstractDataSource *usedDataSource;
    if (dataSource)
        usedDataSource = dataSource;
    else
        usedDataSource = IqOrmCore::dataSource();

    if (!usedDataSource) {
        if (error)
            *error = QObject::tr("Not found any Data Source. Use IQORMCore::setDataSource() first.");
        return false;
    }

    if (!usedDataSource->objectsModelDataSource()) {
        if (error)
            *error = QObject::tr("Not found any Objects Model Data Source. Maybe Data Source broken, check Data Source.");
        return false;
    }

    if (!childOrmModel) {
        if (error)
            *error = QObject::tr("Not found valid child objects ORM model.");
        return false;
    }

    IqOrmTransactionControl transactionControl = usedDataSource->transaction();

    IqOrmDataSourceOperationResult result = usedDataSource->objectsModelDataSource()->truncateModel(childOrmModel);

    if (result) {
        transactionControl.commit();
    } else {
        if (error)
            *error = result.error();
    }

    return result;
}
