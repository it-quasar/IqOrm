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

#ifndef IQORMSQLJOINOPERATION_H
#define IQORMSQLJOINOPERATION_H

#include <QObject>
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlJoinOperation
{
public:
    enum Type
    {
        Inner,
        LeftOuter,
        RightOuter,
        FullOuter,
        Cross
    };

    IqOrmSqlJoinOperation();

    QString toString() const;

    QString table() const;
    void setTable(const QString &table);

    QString condition() const;
    void setCondition(const QString &condition);

    Type type() const;
    void setType(const Type &type);

private:
    QString m_table;
    QString m_condition;
    Type m_type;
};

#endif // IQORMSQLJOINOPERATION_H
