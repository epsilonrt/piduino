#include "property_p.h"

// ---------------------------------------------------------------------------
//                          Class Property
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
Property::Property (PropertyPrivate &dd) : d_ptr (&dd) {

  d_ptr->q_ptr = this;
}
// ---------------------------------------------------------------------------
Property::Property (const QString & table, QSqlDatabase & database, bool isWritable, QObject * parent) :
  QObject (parent), d_ptr (new PropertyPrivate (table, database, isWritable))  {

  d_ptr->q_ptr = this;
}
// ---------------------------------------------------------------------------
Property::~Property() {}
// ---------------------------------------------------------------------------
bool Property::isWritable() const {

  return d_ptr->isWritable;
}
// ---------------------------------------------------------------------------
bool Property::exists() const {

  return values().contains (id());
}
// ---------------------------------------------------------------------------
int Property::id() const {

  return d_ptr->id;
}
// ---------------------------------------------------------------------------
void Property::setId (int id) {
  Q_D (Property);

  if (d->id != id) {

    d->id = id;
    if (exists()) {

      readFromDatabase();
    }
  }
}
// ---------------------------------------------------------------------------
QString Property::name() const {
  return d_ptr->name;
}
// ---------------------------------------------------------------------------
void Property::setName (const QString & name) {

  if (!isWritable()) {

    if (values().values().contains (name)) {
      setId (values().id (name));
    }
  }
  else {

    d_ptr->name = name;
  }
}
// ---------------------------------------------------------------------------
const EnumString & Property::values() const {
  return d_ptr->values;
}
// ---------------------------------------------------------------------------
QSqlDatabase & Property::database() const {
  return  values().database();
}
// ---------------------------------------------------------------------------
QString Property::table() const {
  return values().table();
}
// ---------------------------------------------------------------------------
void Property::valuesFromDatabase() {
  d_ptr->values.updateFromDatabase();
}
// ---------------------------------------------------------------------------
bool Property::writeToDatabase() {
  Q_D (const Property);

  if (isWritable()) {
    QSqlQuery q (database());

    if (exists()) {
      q.prepare (QString (
                   "UPDATE %1 "
                   "SET name=? "
                   "WHERE  id=?"
                 ).arg (table()));
      q.addBindValue (d->name);
      q.addBindValue (d->id);
    }
    else {
      q.prepare (QString (
                   "INSERT INTO %1 (id,name) "
                   "VALUES (?,?)"
                 ).arg (table()));
      q.addBindValue (d->id);
      q.addBindValue (d->name);
    }
    if (q.exec()) {

      valuesFromDatabase();
      emit updated();
      return true;
    }
  }
  return false;
}
// ---------------------------------------------------------------------------
bool Property::readFromDatabase() {
  Q_D (Property);
  QSqlQuery q (database());
  
  q.prepare(QString ("SELECT name FROM %1 WHERE id=?").arg (table()));
  q.addBindValue (d->id);
  q.exec();
  if (q.next()) {
    QString name = q.value (0).toString();

    if (d->name != name) {
      d->name = name;
      emit changed();
    }
    return true;
  }
  return false;
}
