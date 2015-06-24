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
#error 'iqormlib_impl.h' is not supposed to be included directly. Include 'iqormlib.h' instead.
#endif

#include "iqormobjectfactory.h"
#include "iqormmetamodel.h"
#include <QMetaType>
#include <QDebug>
#include "iqormmetamodelprivateaccessor.h"

using namespace IqOrmPrivate;

template <class T>
void IqOrmLib::ormModelIninitializator()
{
    if(IqOrmLib::debugMode())
        qDebug("Run initialize IqOrmMetaModel for %s", T::staticMetaObject.className());

    IqOrmObjectFactory::registerClass<T>();
    T tempObject;
    const IqOrmMetaModel *staticMetaModel = T::staticOrmMetaModel();
    Q_CHECK_PTR(staticMetaModel);
    IqOrmMetaModel * metaModel = const_cast<IqOrmMetaModel *>(staticMetaModel);
    IqOrmMetaModelPrivateAccessor::setTargetStaticMetaObject(metaModel, &T::staticMetaObject);
    tempObject.initializeOrmMetaModel(metaModel);
    Q_ASSERT_X(IqOrmMetaModelPrivateAccessor::isValid(metaModel),
               Q_FUNC_INFO,
               qPrintable(QObject::tr("IqOrmModel for class %0 not valid.")
                          .arg(T::staticMetaObject.className())));
}

template <class T>
void IqOrmLib::scheduleOrmModelInitialization()
{
    m_ormModelInitializators.append(&ormModelIninitializator<T>);
}
