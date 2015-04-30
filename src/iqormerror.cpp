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

#include "iqormerror.h"
#include <QDebug>

IqOrmError::IqOrmError(QObject *parent) :
    QObject(parent),
    m_text(""),
    m_oldError(false)
{
}

void IqOrmError::setText(const QString &text)
{
    if (m_text != text) {
        m_text = text;

        emit textChanged();

        if (m_oldError != !text.isEmpty())
            emit errorChanged();
        m_oldError = !text.isEmpty();
    }
}

QString IqOrmError::text() const
{
    return m_text;
}

bool IqOrmError::error() const
{
    return !m_text.isEmpty();
}

void IqOrmError::clearError()
{
    setText("");
}
