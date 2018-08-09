#include "node_p.h"
#include "manufacturer.h"

// ---------------------------------------------------------------------------
//                          Class Manufacturer
// ---------------------------------------------------------------------------
class ManufacturerPrivate : public NodePrivate {
  public:
    ManufacturerPrivate (Node * parent) :
      NodePrivate (Node::TypeManufacturer, parent) {
    }
    Q_DECLARE_PUBLIC (Manufacturer);
};
// ---------------------------------------------------------------------------
Manufacturer::Manufacturer (ManufacturerPrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
Manufacturer::Manufacturer (int id, const QString & name, Node * parent) :
  Node (* new ManufacturerPrivate (parent)) {
  QSqlQuery q (database());

  setId (id);
  setName (name);

  q.prepare ("SELECT soc_id "
             "FROM board_model "
             "WHERE id = ?");
  q.addBindValue (id);
  q.exec();

  if (q.next()) {

    //setSocId (q.value (0).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void Manufacturer::childrenFromDatabase() {
  QSqlQuery q (database());
  q.prepare ("SELECT id,name "
             "FROM board_model "
             "WHERE board_family_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  while (q.next()) {

    int i = q.value (0).toInt();
    if (i >= 0) {

      // appendChild (new BoardModelNode (i, q.value (1).toString(), this));
    }
  }
}
// ---------------------------------------------------------------------------
Manufacturer::~Manufacturer() {}
