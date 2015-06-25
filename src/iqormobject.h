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

#ifndef IqOrmObject_H
#define IqOrmObject_H

#include <QObject>
#include "iqorm_global.h"
#include "iqormlib.h"
#include "iqormmetamodel.h"
#include "iqormpointerset.h"
#include "iqormsharedpointerset.h"
#include "iqormlazypointerset.h"
#include "iqormsharedlazypointerset.h"
#include "iqormtransactioncontrol.h"
#include "iqormerror.h"

#define IQORM_REGISTER_CLASS(CLASS_NAME) \
    template<class T> int CLASS_NAME::IqOrmClassAutoRegistrer<T>::m_registered = 0; \
    static const CLASS_NAME::IqOrmClassAutoRegistrer<CLASS_NAME> CLASS_NAME##ClassRegistrator; \
    Q_DECLARE_METATYPE(IqOrmPointerSet<CLASS_NAME>) \
    Q_DECLARE_METATYPE(IqOrmSharedPointerSet<CLASS_NAME>) \
    Q_DECLARE_METATYPE(IqOrmLazyPointerSet<CLASS_NAME>) \
    Q_DECLARE_METATYPE(IqOrmSharedLazyPointerSet<CLASS_NAME>) \
\
    inline uint qHash(const QPointer<CLASS_NAME> &key, uint seed) \
    { \
        return qHash(key.data(), seed); \
    }\

#define IQORM_OBJECT \
    Q_PROPERTY(qint64 objectId READ objectId NOTIFY objectIdChanged) \
    Q_PROPERTY(bool isPersisted READ isPersisted NOTIFY isPersistedChanged) \
    Q_PROPERTY(const IqOrmError * lastError READ lastError CONSTANT) \
    Q_PROPERTY(IqOrmAbstractDataSource * dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged) \
\
private: \
\
    virtual void emitObjectIdChanged() Q_DECL_OVERRIDE {emit objectIdChanged();} \
\
    virtual void emitIsPersistedChanged() Q_DECL_OVERRIDE {emit isPersistedChanged();} \
\
    virtual void emitDataSourceChanged() Q_DECL_OVERRIDE {emit dataSourceChanged();} \
\
public: \
\
    static const IqOrmMetaModel* staticOrmMetaModel() \
    { \
        Q_ASSERT_X(iqOrmClassRegistred(), \
            Q_FUNC_INFO, \
            tr("IqOrmObject not registerd. Check IQORM_REGISTER_CLASS macros for class %0.") \
               .arg(staticMetaObject.className()).toLocal8Bit().constData()); \
        static IqOrmMetaModel* const m_staticOrmMetaModel(new IqOrmMetaModel()); \
        return m_staticOrmMetaModel; \
    }; \
\
    virtual const IqOrmMetaModel * ormMetaModel() const Q_DECL_OVERRIDE {return staticOrmMetaModel();} \
\
    Q_SIGNAL void objectIdChanged(); \
\
    Q_SIGNAL void isPersistedChanged(); \
\
    Q_SIGNAL void dataSourceChanged(); \
\
    Q_INVOKABLE bool load(const qint64 objectId, IqOrmTransactionControl transaction = IqOrmTransactionControl()) {return IqOrmObject::load(objectId, transaction);} \
\
    Q_INVOKABLE bool reload(IqOrmTransactionControl transaction = IqOrmTransactionControl()) {return IqOrmObject::reload(transaction);} \
\
    Q_INVOKABLE bool save(IqOrmTransactionControl transaction = IqOrmTransactionControl()) {return IqOrmObject::save(transaction);} \
\
    Q_INVOKABLE bool persist(IqOrmTransactionControl transaction = IqOrmTransactionControl()) {return IqOrmObject::persist(transaction);} \
\
    Q_INVOKABLE bool remove(IqOrmTransactionControl transaction = IqOrmTransactionControl()) {return IqOrmObject::remove(transaction);} \
\
    template<class T> \
    class IqOrmClassAutoRegistrer \
    { \
    public: \
        IqOrmClassAutoRegistrer() \
        { \
            if (m_registered++ == 0) { \
                iqOrmClassRegistred(true); \
                IqOrmLib::scheduleOrmModelInitialization<T>(); \
            } \
        } \
    private: \
        static int m_registered; \
    }; \
\
private: \
    static bool iqOrmClassRegistred(bool registred = false) \
    {\
        static bool m_registred (false); \
        if (registred) \
            m_registred = true; \
        return m_registred; \
    }



class IqOrmAbstractTriggers;
class IqOrmPropertyDescription;
class IqOrmAbstractDataSource;
class IqOrmSqlDataSource;
class IqOrmLocalSynchronizer;
class IqOrmMetaModel;
class IqOrmError;
class IqOrmObjectRawData;
namespace IqOrmPrivate {
    class IqOrmObjectPrivateAccessor;
}

class IQORMSHARED_EXPORT IqOrmObject
{
#ifdef Q_QDOC
    Q_PROPERTY(qint64 objectId READ objectId NOTIFY objectIdChanged)
    Q_PROPERTY(bool isPersisted READ isPersisted NOTIFY isPersistedChanged)
    Q_PROPERTY(const IqOrmError * lastError READ lastError CONSTANT)
    Q_PROPERTY(IqOrmAbstractDataSource * dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged)
signals:
    void objectIdChanged();
    void isPersistedChanged();
    void dataSourceChanged();
public:
    static const IqOrmMetaModel* staticOrmMetaModel();
#endif

public:
    explicit IqOrmObject();

#ifndef Q_QDOC
    virtual ~IqOrmObject();
#endif

    virtual void initializeOrmMetaModel(IqOrmMetaModel *model) const = 0;

    virtual const IqOrmMetaModel *ormMetaModel() const;

    bool load(const qint64 objectId, IqOrmTransactionControl transaction = IqOrmTransactionControl());

    bool load(IqOrmTransactionControl transaction = IqOrmTransactionControl());

    bool reload(IqOrmTransactionControl transaction = IqOrmTransactionControl());

    bool save(IqOrmTransactionControl transaction = IqOrmTransactionControl());

    bool persist(IqOrmTransactionControl transaction = IqOrmTransactionControl());

    bool remove(IqOrmTransactionControl transaction = IqOrmTransactionControl());

protected:
    virtual IqOrmAbstractTriggers *triggers() const;

public:
    qint64 objectId() const;

    bool isPersisted() const;

    IqOrmAbstractDataSource *dataSource() const;
    void setDataSource(IqOrmAbstractDataSource* dataSource);

    const IqOrmError *lastError() const;

#ifndef Q_QDOC
protected:
    virtual void emitObjectIdChanged() = 0;
    virtual void emitIsPersistedChanged() = 0;
    virtual void emitDataSourceChanged() = 0;
#endif

private:
    friend class IqOrmMetaModel;
    friend class IqOrmPrivate::IqOrmObjectPrivateAccessor;

    void setObjectId(qint64 objectId);

    IqOrmAbstractDataSource *usedDataSource() const;

    void updateSourcePropertyValues();

    void clearSourcePropertyValues();

    QSet<const IqOrmPropertyDescription *> sourcePropertyChanges() const;

    QVariant sourcePropertyValue(const IqOrmPropertyDescription *propertyDescription) const;

    bool isLoadedFromDataSource() const;
    void setIsLoadedFromDataSource(bool isLoadedFromDataSource);

    bool isSavedToDataSource() const;

    void setValues(const IqOrmObjectRawData &values);

private:
    qint64 m_objectId;
    IqOrmAbstractDataSource* m_dataSource;
    IqOrmError *m_lastError;
    QHash<const IqOrmPropertyDescription *, QVariant> m_sourcePropertyValues;
    bool m_isLoadedFromDataSource;
};

#endif // IqOrmObject_H
