//! [include]
#include "person.h"
#include <QDebug>

//! [include]

void test () {
    {
        //! [load]
        Person alex;
        if (!alex.load(12))
            qWarning("Error on load: %s",
                     qPrintable(alex.lastError()->text()));

        qDebug("Person with name %s loaded. And him %d years old.",
               qPrintable(alex.name()),
               alex.age());
        //! [load]
    }

    {
        //! [save]
        Person alex;
        alex.setName("Alex");
        alex.setAge(22);

        if (!alex.save())
            qWarning("Error on save: %s",
                     qPrintable(alex.lastError()->text()));

        qDebug("Person with name %s saved. New objectId %d",
               qPrintable(alex.name()),
               alex.objectId());
        //! [save]
    }

    {
        //! [update]
        Person alex;
        alex.load(12);

        int oldObjectId = alex.objectId();

        alex.setAge(22);

        if (!alex.save())
            qWarning("Error on update: %s",
                     qPrintable(alex.lastError()->text()));

        qDebug("Person with name %s updated. Old objectId %d == new objectId %d",
               qPrintable(alex.name()),
               oldObjectId,
               alex.objectId());
        //! [update]
    }

    {
        //! [remove]
        Person alex;
        alex.load(12);

        if (!alex.remove())
            qWarning("Error on remove: %s",
                     qPrintable(alex.lastError()->text()));

        qDebug("Person with name %s removed.",
               qPrintable(alex.name()));
        //! [remove]
    }

}
