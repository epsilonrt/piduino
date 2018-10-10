#include <QtCore>
#include <QtSql>
#include "enumstring.h"

// ---------------------------------------------------------------------------
//                          Class EnumString
// ---------------------------------------------------------------------------
EnumString::EnumString (const QString & table, QSqlDatabase & db) :
  m_database (db), m_table (table) {

  updateFromDatabase();
}
// ---------------------------------------------------------------------------
void EnumString::updateFromDatabase() {
  QString select = QString ("SELECT id,name FROM %1").arg (m_table);
  QSqlQuery q (select, m_database);

  q.exec();
  clear();
  while (q.next()) {
    insert (q.value (0).toInt(), q.value (1).toString());
  }
}
// ---------------------------------------------------------------------------
QString EnumString::name (int id) const {
  return value (id, QObject::tr ("Unknown"));
}
// ---------------------------------------------------------------------------
int EnumString::id (const QString & name) const {
  return key (name, -1);
}
// ---------------------------------------------------------------------------
QSqlDatabase & EnumString::database() const {
  return m_database;
}
// ---------------------------------------------------------------------------
QString EnumString::table() const {
  return m_table;
}
