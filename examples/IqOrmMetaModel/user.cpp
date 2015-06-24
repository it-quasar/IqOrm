#include "user.h"

User::User()
{

}

//! [initializeOrmMetaModel]
void User::initializeOrmMetaModel(IqOrmMetaModel *model) const
{
    model->setTableName("users");
    model->setDirect("name");
    model->setDirect("surname");
    model->setDirect("age");
    //Each user is in the same group
    model->setManyToOne<Group>("group", "group_id");
    //Each user is leading many projects
    model->setOneToMany<Project>("projects", "owner");
}
//! [initializeOrmMetaModel]
