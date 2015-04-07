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

#ifndef IQORMLIB_H
#define IQORMLIB_H

#include "iqorm_global.h"
#include "iqormobjectfactory.h"
#include <QCoreApplication>
#include "iqormmetamodel.h"
#include <QAtomicInt>
#include <QDebug>

class IQORMSHARED_EXPORT IqOrmLib
{
public:
    IqOrmLib();

    static void initialize();

    template <class T>
    static void scheduleOrmModelInitialization();

private:
    template <class T>
    static void ormModelIninitializator();

private:
    typedef void (*initializatorMember)();

    static QList<initializatorMember> m_ormModelInitializators;
};

template <class T>
void IqOrmLib::ormModelIninitializator()
{
    IqOrmObjectFactory::registerClass<T>();
    T tempObject;
    const IqOrmMetaModel *staticMetaModel = T::staticOrmMetaModel();
    Q_CHECK_PTR(staticMetaModel);
    IqOrmMetaModel * metaModel = const_cast<IqOrmMetaModel *>(staticMetaModel);
    metaModel->setTargetStaticMetaObject(&T::staticMetaObject);
    tempObject.initializeOrmMetaModel(metaModel);
    Q_ASSERT_X(metaModel->isValid(),
               Q_FUNC_INFO,
               QObject::tr("IqOrmModel for class %0 not valid.")
               .arg(T::staticMetaObject.className()).toLocal8Bit().constData());
}

template <class T>
void IqOrmLib::scheduleOrmModelInitialization()
{
    m_ormModelInitializators.append(&ormModelIninitializator<T>);
}

#endif // IQORMLIB_H
