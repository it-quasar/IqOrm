#ifndef USER_H
#define USER_H

#include <QObject>
#include "iqormobject.h"

class User : public QObject, public IqOrmObject
{
public:
    User();

public:
    virtual void initializeOrmMetaModel(IqOrmMetaModel *model) const;
};

#endif // USER_H
