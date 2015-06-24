#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <IqOrmObject>

//! [IQORM_OBJECT_1]
//! [IQORM_REGISTER_CLASS_1]
//! [initializeOrmMetaModel_1]
class Person : public QObject, public IqOrmObject
{
//! [IQORM_REGISTER_CLASS_1]
    Q_OBJECT
    IQORM_OBJECT
//! [IQORM_OBJECT_1]
//! [initializeOrmMetaModel_1]
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int age READ age WRITE setAge NOTIFY ageChanged)

//! [IQORM_OBJECT_2]
//! [initializeOrmMetaModel_2]
public:
//! [initializeOrmMetaModel_2]
    explicit Person(QObject *parent = 0);
//! [IQORM_OBJECT_2]

//! [initializeOrmMetaModel_3]
    virtual void initializeOrmMetaModel(IqOrmMetaModel *model) const;
//! [initializeOrmMetaModel_3]

public:
    QString name() const;
    void setName(const QString &name);

    int age() const;
    void setAge(int age);

signals:
    void nameChanged();
    void ageChagned();

private:
    QString m_name;
    int m_age;
//! [IQORM_OBJECT_3]
//! [IQORM_REGISTER_CLASS_2]
};
//! [IQORM_OBJECT_3]

IQORM_REGISTER_CLASS(Person)
//! [IQORM_REGISTER_CLASS_2]

#endif // PERSON_H
