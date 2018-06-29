#ifndef PROPERTY_PRIVATE_H
#define PROPERTY_PRIVATE_H

#include <QtCore>
#include <QtSql>
#include "property.h"

class PropertyPrivate {
  public:
    PropertyPrivate (const QString & tableName, QSqlDatabase & database, bool writable) :
      id (-1), values (tableName, database), q_ptr (0), isWritable (writable) {}

    int id;
    QString name;
    EnumString values;
    Property * q_ptr;
    bool isWritable;
    Q_DECLARE_PUBLIC (Property);
};
#endif
