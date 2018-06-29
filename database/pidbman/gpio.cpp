#include <QtCore>
#include <QtSql>
#include "node.h"
#include "gpio.h"
#include "connector.h"

// ---------------------------------------------------------------------------
//                          Class GpioNode
// ---------------------------------------------------------------------------
GpioNode::GpioNode (int id, const QString & name, Node * parent) :
  Node (Node::TypeGpio, parent) {

  setId (id);
  setName (name);
  childrenFromDatabase();
}

// ---------------------------------------------------------------------------
void GpioNode::childrenFromDatabase() {
  QSqlQuery q (database());
  q.prepare ("SELECT gpio_connector_id,num "
             "FROM gpio_has_connector "
             "WHERE gpio_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  while (q.next()) {

    int i = q.value (0).toInt();
    if (i >= 0) {

      appendChild (new ConnectorNode (i, q.value (1).toInt(), this));
    }
  }
}
// ---------------------------------------------------------------------------
int GpioNode::variantId() const {
  return parent()->id();
}
