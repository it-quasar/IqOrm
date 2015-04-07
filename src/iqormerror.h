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

#ifndef IQORMERROR_H
#define IQORMERROR_H

#include <QObject>
#include <QStringList>
#include "iqorm_global.h"


class IQORMSHARED_EXPORT IqOrmError : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(bool error READ error NOTIFY errorChanged)

public:
    explicit IqOrmError(QObject *parent = Q_NULLPTR);

    void clearError();

public:
    QString text() const;

    bool error() const;

signals:
    void textChanged();
    void errorChanged();

private:
    friend class IqOrmObject;
    friend class IqOrmBaseModel;

    void setText(const QString &text);

private:
    QString m_text;
    bool m_oldError;
};

#endif // IQORMERROR_H
