#ifndef PROPERTY_PRIVATE_H
#define PROPERTY_PRIVATE_H

#include <QtCore>
#include <QtSql>
#include "property.h"

class PropertyPrivate {
  public:
    PropertyPrivate (Node * parent, Property * q);

    Property * q_ptr;
    Node * parent;
    int id;
    QString table;
    Q_DECLARE_PUBLIC (Property);
};
#endif
