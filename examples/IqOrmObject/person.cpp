#include "person.h"

Person::Person(QObject *parent) :
    QObject(parent),
    IqOrmObject(),
    m_name(""),
    m_age(-1)
{
}

//! [initializeOrmMetaModel_1]
void Person::initializeOrmMetaModel(IqOrmMetaModel *model) const
{
    model->setTableName("person");
    model->setPropertyColumn("name");
    model->setPropertyColumn("age");
}
//! [initializeOrmMetaModel_1]

QString Person::name() const
{
    return m_name;
}

void Person::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

int Person::age() const
{
    return m_age;
}

void Person::setAge(int age)
{
    if (m_age != age) {
        m_age = age;
        emit ageChagned();
    }
}

