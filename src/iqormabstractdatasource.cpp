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

#include "iqormabstractdatasource.h"
#include "iqormtransactioncontrol.h"

/*!
 * \class IqOrmAbstractDataSource
 * \since 5.4
 * \brief Класс, описывающий источник данных
 *
 * На основе данного класса проектируются все остальные классы источников данных.
 */

/*!
 * \brief Конструктор
 * \param parent Родитель
 */
IqOrmAbstractDataSource::IqOrmAbstractDataSource(QObject *parent) :
    QObject(parent)
{
}

IqOrmAbstractDataSource::~IqOrmAbstractDataSource()
{
}

/*!
 * \brief Открывает транзакцию для источника данных
 *
 * Данный метод открывает транзацию для источника данных и возвращает объект \ref IqOrmTransactionControl, который используется
 * для управления данной транзакцией. В дальнейшем все операции с источником данных будут проходить в одной транзакции,
 * чтоб принять их нужно будет выполнить метод \ref IqOrmTransactionControl::commit(), чтоб отклонить, нужно выполнить
 * метод \ref IqOrmTransactionControl::rollback(). Так же, если транзация не будет принята, то по удалению объекта
 * \ref IqOrmTransactionControl, будет произведен автоматический отказ от изменений, что эквивалентно методу
 * \ref IqOrmTransactionControl::rollback().
 *
 * ~~~~~~
 *     IqOrmTransactionControl transactionControl = dataSource->transaction();
 *     //Какие-то операции с источником данных, которые мы хотим выполнить в одной транзакции
 *     transactionControl->commit();
 * ~~~~~~
 *
 * Не каждый источник данных обязан поддерживать транзации. Работа с ними должна быть реализована в наследниках данного
 * класса, в метода \ref openTransaction(), \ref closeTransaction().
 *
 * \return Объект для управления транзакцией
 */
IqOrmTransactionControl IqOrmAbstractDataSource::transaction()
{
    return IqOrmTransactionControl(this);
}

bool IqOrmAbstractDataSource::openTransaction()
{
    return false;
}

bool IqOrmAbstractDataSource::commitTransaction()
{
    return false;
}

bool IqOrmAbstractDataSource::rollbackTransaction()
{
    return false;
}
