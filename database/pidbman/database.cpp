#include "node_p.h"
#include "database.h"
#include "folder.h"

// ---------------------------------------------------------------------------
//                          Class Database
// ---------------------------------------------------------------------------
class DatabasePrivate : public NodePrivate {
  public:
    DatabasePrivate (QSqlDatabase & database) :
      NodePrivate (Node::TypeDatabase, 0), db (database) {
    }
    QSqlDatabase & db;
    Q_DECLARE_PUBLIC (Database);
};
// ---------------------------------------------------------------------------
Database::Database (DatabasePrivate & dd) : Node (dd) {}

// ---------------------------------------------------------------------------
Database::Database (QSqlDatabase & db, QObject * parent) :
  Node (*new DatabasePrivate (db))  {
  Q_D (Database);

  d->q_ptr = this;
  QObject::setParent (parent);

  setName (db.databaseName());

  childrenFromDatabase();
}

// ---------------------------------------------------------------------------
Database::~Database() {}

// ---------------------------------------------------------------------------
void Database::childrenFromDatabase() {

  clearChildren();
  appendChild (new Folder (Node::TypeBoardFamily, tr ("Board"), this));
  appendChild (new Folder (Node::TypeGpio, tr ("GpioNode"), this));
  appendChild (new Folder (Node::TypeSocFamily, tr ("SocNode"), this));
  appendChild (new Folder (Node::TypeManufacturer, tr ("Manufacturer"), this));
}

// ---------------------------------------------------------------------------
QSqlDatabase & Database::database() const {
  Q_D (const Database);
  return  d->db;
}

// ---------------------------------------------------------------------------
Folder * Database::folder (Database::SubFolder f) {
  return reinterpret_cast<Folder *> (children() [static_cast<int> (f)]);
}
