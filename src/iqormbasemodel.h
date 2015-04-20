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

#ifndef IQORMBASEMODEL_H
#define IQORMBASEMODEL_H

#include <QAbstractTableModel>
#include <QPointer>
#include <QSet>
#include <QMetaMethod>
#include "iqorm_global.h"
#include "iqormfilter.h"
#include "iqormobjectrawdata.h"

class IqOrmMetaModel;
class IqOrmObject;
class IqOrmAbstractDataSource;
class IqOrmError;
namespace IqOrmPrivate {
class IqOrmModelPrivateAccessor;
}

class IQORMSHARED_EXPORT IqOrmBaseModel : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(IqOrmAbstractFilter * filters READ filters WRITE setFilters NOTIFY filtersChanged)
    Q_PROPERTY(IqOrmError * lastError READ lastError CONSTANT)
    Q_PROPERTY(qint64 count READ count NOTIFY countChanged)

public:
    enum OrderBy
    {
        Asc,
        Desc
    };

    explicit IqOrmBaseModel(QObject *parent = Q_NULLPTR);

    virtual ~IqOrmBaseModel();

    virtual const IqOrmMetaModel * childsOrmMetaModel() const = 0;


    Q_INVOKABLE virtual QObject *get(int row) const;

    Q_INVOKABLE virtual QObject *first() const;

    Q_INVOKABLE virtual QObject *last() const;

    virtual IqOrmObject *take(int row);

    virtual IqOrmObject *take(IqOrmObject *object);

    void setPropertyEditable(const QString &property,
                             const bool editable = true);

    void showProperty(const QString &property,
                      const QString &headerTitle = QString());

    void hideProperty(const QString &property);

    void setPropertyHeader(const QString &property,
                           const QString &headerTitle);

    bool find(const QString &propertyName,
              IqOrmFilter::Operation operation,
              const QVariant &value,
              IqOrmAbstractDataSource *dataSource = Q_NULLPTR);

    bool find(const QString &propertyName,
              IqOrmFilter::Operation operation,
              const QVariant &value,
              Qt::CaseSensitivity caseSensitivity,
              IqOrmAbstractDataSource *dataSource = Q_NULLPTR);

    Q_INVOKABLE bool load(IqOrmAbstractDataSource *dataSource);

    Q_INVOKABLE bool load(qint64 limit = -1,
                          qint64 offset = 0,
                          OrderBy orderBy = Asc,
                          IqOrmAbstractDataSource *dataSource = Q_NULLPTR);

    Q_INVOKABLE bool loadFirst(qint64 count,
                               IqOrmAbstractDataSource *dataSource = Q_NULLPTR);

    Q_INVOKABLE bool loadLast(qint64 count,
                              IqOrmAbstractDataSource *dataSource = Q_NULLPTR);

    Q_INVOKABLE bool truncate(IqOrmAbstractDataSource *dataSource = Q_NULLPTR);


    qint64 count() const;

    int rowOf(const IqOrmObject* object) const;

    QList<IqOrmObject *> toObjectList() const;

    IqOrmAbstractDataSource *lastDataSource() const;

    void clear();


    virtual QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual QHash<int, QByteArray> roleNames() const Q_DECL_OVERRIDE;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    virtual bool moveRows(const QModelIndex &sourceParent,
                          int sourceRow,
                          int count,
                          const QModelIndex &destinationParent,
                          int destinationChild) Q_DECL_OVERRIDE;

    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual bool setData(const QModelIndex &index,
                         const QVariant &value,
                         int role = Qt::EditRole) Q_DECL_OVERRIDE;

    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) Q_DECL_OVERRIDE;

signals:
    void invisibleDataChanged(int row, const QString &property);

protected:
    virtual IqOrmObject * createChildObject() const = 0;

    void addPropertySiagnalIndex(const QString& property, int signalIndex);
    static bool processTruncate(const IqOrmMetaModel *childOrmModel,
                                IqOrmAbstractDataSource *dataSource,
                                QString *error);

public:
    IqOrmError *lastError() const;

    IqOrmAbstractFilter *filters() const;
    void setFilters(IqOrmAbstractFilter *filters);

    bool childChangeMonitoring() const;
    void setChildChangeMonitoring(bool childChangeMonitoring);

signals:
    void filtersChanged();
    void countChanged();

private slots:
    void onObjectChanged();

private:
    friend class IqOrmPrivate::IqOrmModelPrivateAccessor;
    class IqOrmModelItem
    {
    public:
        explicit IqOrmModelItem();
        IqOrmObjectRawData rawData;
        IqOrmObject *object;
        QObject *qobject;
    };


    void insertItems(qint64 row, QList<IqOrmModelItem *> items);
    void setObjectsValues(const QList<IqOrmObjectRawData> &objectValues);

    void enableChildMonitoring(IqOrmObject *child) const;
    void disableChildMonitoring(IqOrmObject *child) const;

    void createItemObject(IqOrmModelItem *item) const;

private:

    QMetaMethod m_onObjectChangedMethod;
    IqOrmAbstractFilter *m_filters;
    IqOrmError *m_lastError;
    QPointer<IqOrmAbstractDataSource> m_lastDataSource;

    QList<IqOrmModelItem *> m_items;
    QHash<const IqOrmObject *, qint64> m_objectRows;

    QStringList m_visibleProperties;
    QSet<QString> m_editableProperties;
    //1 - имя свойства, 2 - его заголовок
    QHash<QString, QString> m_propertyHeaderTitles;
    QHash<int, QString> m_signalIndexProperty;
    bool m_childChangeMonitoring;
};

#endif // IQORMBASEMODEL_H
