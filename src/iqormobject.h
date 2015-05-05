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
#include "iqormlazysharedpointerset.h"

#define IQORM_REGISTER_CLASS(CLASS_NAME) \
    template<class T> int CLASS_NAME::IqOrmClassAutoRegistrer<T>::m_registered = 0; \
    static const CLASS_NAME::IqOrmClassAutoRegistrer<CLASS_NAME> CLASS_NAME##ClassRegistrator; \
    Q_DECLARE_METATYPE(IqOrmPointerSet<CLASS_NAME>) \
    Q_DECLARE_METATYPE(IqOrmSharedPointerSet<CLASS_NAME>) \
    Q_DECLARE_METATYPE(IqOrmLazyPointerSet<CLASS_NAME>) \
    Q_DECLARE_METATYPE(IqOrmLazySharedPointerSet<CLASS_NAME>) \
\
    inline uint qHash(const QPointer<CLASS_NAME> &key, uint seed) \
    { \
        return qHash(key.data(), seed); \
    }\

#define IQORM_OBJECT \
    Q_PROPERTY(qint64 objectId READ objectId NOTIFY objectIdChanged) \
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged) \
    Q_PROPERTY(const IqOrmError * lastError READ lastError CONSTANT) \
    Q_PROPERTY(IqOrmAbstractDataSource * dataSource READ dataSource WRITE setDataSource NOTIFY dataSourceChanged) \
\
private: \
\
    virtual void emitObjectIdChanged() Q_DECL_OVERRIDE {emit objectIdChanged();} \
\
    virtual void emitIsValidChanged() Q_DECL_OVERRIDE {emit isValidChanged();} \
\
    virtual void emitDataSourceChanged() Q_DECL_OVERRIDE {emit dataSourceChanged();} \
\
public: \
\
    static const IqOrmMetaModel* staticOrmMetaModel() \
    { \
        Q_ASSERT_X(iqormClassRegistred(), \
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
    Q_SIGNAL void isValidChanged(); \
\
    Q_SIGNAL void dataSourceChanged(); \
\
    Q_INVOKABLE bool load(const qint64 objectId, bool autoTransaction = true) {return IqOrmObject::load(objectId, autoTransaction);} \
\
    Q_INVOKABLE bool reload(bool autoTransaction = true) {return IqOrmObject::reload(autoTransaction);} \
\
    Q_INVOKABLE bool save(bool autoTransaction = true) {return IqOrmObject::save(autoTransaction);} \
\
    Q_INVOKABLE bool remove(bool autoTransaction = true) {return IqOrmObject::remove(autoTransaction);} \
\
    template<class T> \
    class IqOrmClassAutoRegistrer \
    { \
    public: \
        IqOrmClassAutoRegistrer() \
        { \
            if (m_registered++ == 0) { \
                iqormClassRegistred(true); \
                IqOrmLib::scheduleOrmModelInitialization<T>(); \
            } \
        } \
    private: \
        static int m_registered; \
    }; \
\
private: \
    static bool iqormClassRegistred(bool registred = false) \
    {\
        static bool m_registred (false); \
        if (registred) \
        m_registred = true; \
        return m_registred; \
    }




class IqOrmPropertyDescription;
class IqOrmAbstractDataSource;
class IqOrmSqlDataSource;
class IqOrmLocalSynchronizer;
class IqOrmMetaModel;
class IqOrmError;
class IqOrmObjectRawData;

class IQORMSHARED_EXPORT IqOrmObject
{
public:
    explicit IqOrmObject(IqOrmAbstractDataSource* dataSource = Q_NULLPTR);

    virtual ~IqOrmObject();

    /*!
     * \brief Инециализирует ORM-модель, связаную с данным объектом
     *
     * Для работы ORM, используются специальные модели, в которых описано какие параметры и как
     * храняться в источниках данных. Данный метод вызывается объектом, имеющим интерфейсы ORM,
     * для того чтобы инециализировать модель, связанную с ним. Модель инециализирутся только один
     * раз и в дальнейшем она будет использоваться для любых объектов того же класса, поэтому данный
     * метод выполниться только один раз в течении работы программы.
     *
     * \param model Указатель на ORM-модель
     */
    virtual void initializeOrmMetaModel(IqOrmMetaModel *model) const = 0;

    virtual const IqOrmMetaModel *ormMetaModel() const;

    bool load(const qint64 objectId, bool autoTransaction = true);

    bool load(bool autoTransaction = true);

    bool reload(bool autoTransaction = true);

    bool save(bool autoTransaction = true);

    bool remove(bool autoTransaction = true);

protected:
    /*!
     * \brief Сбрасывает все параметры объекта
     *
     * Сбрасывает все параметры объекта на значения по-умолчанию. Данный метод должен быть обязательно
     * переопределен в наследниках класса.
     */
    virtual void reset() = 0;

    IqOrmAbstractTriggers *triggers() const;

public:
    qint64 objectId() const;

    bool isValid() const;

    IqOrmAbstractDataSource *dataSource() const;
    void setDataSource(IqOrmAbstractDataSource* dataSource);

    const IqOrmError *lastError() const;

protected:
    virtual void emitObjectIdChanged() = 0;
    virtual void emitIsValidChanged() = 0;
    virtual void emitDataSourceChanged() = 0;

private:
    friend class IqOrmMetaModel;
    friend class IqOrmPrivate::IqOrmObjectPrivateAccessor;

    void setObjectId(qint64 objectId);

    IqOrmAbstractDataSource *usedDataSource() const;

    /*!
     * \brief Обновляет сохраненные свойства объекта из источника
     */
    void updateSourcePropertyValues();

    /*!
     * \brief Отчищает сохраненные свойства объекта из источника
     */
    void clearSourcePropertyValues();

    /*!
     * \brief Возвращает свойства объекта, которые были изменены
     *
     * Сравнивает текущие свойства объекта со свойствами объекта из источника данных и возвращает
     * названия свойств объекта отличных от свойст объекта в источнике данных.
     *
     * \return Список свойств объекта
     */
    QList<const IqOrmPropertyDescription *> sourcePropertyChanges() const;

    /*!
     * \brief Возвращает первоночальное (сохраненное в источнике данных) свойство объекта
     * \param propertyDescription Описание свойства
     * \return Свойство объекта
     */
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
