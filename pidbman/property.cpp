#include "property_p.h"
#include "node.h"

// ---------------------------------------------------------------------------
//                          Class Property
// ---------------------------------------------------------------------------
/*

  switch (type) {
    case Property::TypeBoardFamily:
      table = QString ("board_family");
      break;
    case Property::TypeBoardModel:
      table = QString ("board_model");
      break;
    case Property::TypeBoardVariant:
      table = QString ("board");
      break;
    case Property::TypeGpio:
      table = QString ("gpio");
      break;
    case Property::TypeConnectorFamily:
      table = QString ("gpio_connector_family");
      readOnly = true;
      break;
    case Property::TypeConnector:
      table = QString ("gpio_connector");
      break;
    case Property::TypePin:
      table = QString ("gpio_pin");
      break;
    case Property::TypePinType:
      table = QString ("gpio_pin_type");
      readOnly = true;
      break;
    case Property::TypePinMode:
      table = QString ("gpio_pin_mode");
      readOnly = true;
      break;
    case Property::TypePinName:
      table = QString ("gpio_pin_name");
      break;
    case Property::TypeFunction:
      table = QString ("gpio_pin_has_name");
      break;
    case Property::TypeSocFamily:
      table = QString ("soc_family");
      readOnly = true;
      break;
    case Property::TypeSoc:
      table = QString ("soc");
      break;
    case Property::TypeManufacturer:
      table = QString ("manufacturer");
      break;
    case Property::TypeArch:
      table = QString ("arch");
      readOnly = true;
      break;
  } */
PropertyPrivate::PropertyPrivate (Node * p, Property * q) :
  q_ptr (q), parent (p), id (-1) {
}
// ---------------------------------------------------------------------------
Property::Property (PropertyPrivate &dd) : d_ptr (&dd) {
}
// ---------------------------------------------------------------------------
Property::Property (Node * parent) :
  QObject (0), d_ptr (new PropertyPrivate (parent, this))  {
}
// ---------------------------------------------------------------------------
Property::~Property() {
}
// ---------------------------------------------------------------------------
bool Property::isWritable() const {

  return false;
}
// ---------------------------------------------------------------------------
bool Property::hasName() const {
  return false;
}
// ---------------------------------------------------------------------------
bool Property::exists() const {

  return exists (id());
}
// ---------------------------------------------------------------------------
bool Property::exists (int id) const {
  QSqlQuery q (database());

  q.prepare (QString ("SELECT 1 FROM %1 WHERE id=?").arg (table()));
  q.addBindValue (id);
  if (q.exec()) {
    return q.size() == 1;
  }
  return false;
}
// ---------------------------------------------------------------------------
int Property::id() const {

  return d_ptr->id;
}
// ---------------------------------------------------------------------------
QList<int> Property::identifiers() const {
  Q_D (const Property);
  QList<int> list;
  QSqlQuery q (database());

  q.prepare (QString ("SELECT id FROM %2").arg (d->table));

  if (q.exec()) {
    while (q.next()) {
      list.append (q.value (0).toInt());
    }
  }
  return list;
}
// ---------------------------------------------------------------------------
QList<QString> Property::names() const {
  Q_D (const Property);
  QList<QString> list;
  QSqlQuery q (database());

  q.prepare (QString ("SELECT name FROM %2").arg (d->table));

  if (q.exec()) {
    while (q.next()) {
      list.append (q.value (0).toString());
    }
  }
  return list;
}
// ---------------------------------------------------------------------------
bool Property::exists (const QString & n) const {
  QSqlQuery q (database());

  q.prepare (QString ("SELECT 1 FROM %1 WHERE name=?").arg (table()));
  q.addBindValue (n);
  if (q.exec()) {
    return q.size() == 1;
  }
  return false;
}
// ---------------------------------------------------------------------------
QIcon Property::icon() const {
  return QIcon();
}
// ---------------------------------------------------------------------------
QString Property::toolTip() const {
  return QString();
}
// ---------------------------------------------------------------------------
void Property::setId (int id) {
  Q_D (Property);

  d->id = id;
  if (exists()) {

    readFromDatabase();
  }
}
// ---------------------------------------------------------------------------
QString Property::name() const {
  return QString();
}
// ---------------------------------------------------------------------------
void Property::setName (const QString & name) {
}
// ---------------------------------------------------------------------------
QSqlDatabase & Property::database() const {
  return  d_ptr->parent->database();
}
// ---------------------------------------------------------------------------
QString Property::table() const {
  return d_ptr->table;
}
// ---------------------------------------------------------------------------
bool Property::writeToDatabase() {
  Q_D (const Property);

  if (isWritable()) {
    QSqlQuery q (database());

    if (exists()) {
      if (hasName()) {
        q.prepare (QString (
                     "UPDATE %1 "
                     "SET name=? "
                     "WHERE  id=?"
                   ).arg (table()));
        q.addBindValue (name());
        q.addBindValue (d->id);
      }
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

  q.prepare (QString ("SELECT name FROM %1 WHERE id=?").arg (table()));
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
// ---------------------------------------------------------------------------
bool Property::deleteToDatabase() {
  Q_D (Property);
  QSqlQuery q (database());

  q.prepare (QString ("DELETE FROM %1 WHERE id=?").arg (table()));
  q.addBindValue (d->id);
  return q.exec();
}
