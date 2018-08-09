#include "node_p.h"
#include "connector.h"
#include "pin.h"

// ---------------------------------------------------------------------------
//                          Class ConnectorNode
// ---------------------------------------------------------------------------
class ConnectorNodePrivate : public NodePrivate {
  public:
    ConnectorNodePrivate (Node * parent) :
      NodePrivate (Node::TypeConnector, parent), number (-1), rows (0),
      family (parent->database(), 0)  {
    }
    int number;
    int rows;
    ConnectorFamily family;
    Q_DECLARE_PUBLIC (ConnectorNode);
};
// ---------------------------------------------------------------------------
ConnectorNode::ConnectorNode (ConnectorNodePrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
ConnectorNode::ConnectorNode (int i, int n, Node * parent) :
  Node (* new ConnectorNodePrivate (parent)) {
  QSqlQuery q (database());

  setId (i);
  setNumber (n);
  q.prepare ("SELECT name,rows,gpio_connector_family_id "
             "FROM gpio_connector "
             "WHERE id = ?");
  q.addBindValue (i);
  q.exec();

  if (q.next()) {

    setName (q.value (0).toString());
    setRows (q.value (1).toInt());
    family().setId (q.value (2).toInt());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void ConnectorNode::childrenFromDatabase() {
  QSqlQuery q (database());
  q.prepare ("SELECT gpio_pin_id,row,column "
             "FROM gpio_connector_has_pin "
             "WHERE gpio_connector_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  while (q.next()) {

    int i = q.value (0).toInt();
    if (i >= 0) {
      PinNode * p = new PinNode (i, this);
      p->setRow (q.value (1).toInt());
      p->setColumn (q.value (2).toInt());
      appendChild (p);
    }
  }
}
// ---------------------------------------------------------------------------
ConnectorNode::~ConnectorNode() {}
// ---------------------------------------------------------------------------
int ConnectorNode::gpioId() const {
  return parent()->id();
}
// ---------------------------------------------------------------------------
int ConnectorNode::number() const {
  Q_D (const ConnectorNode);
  return d->number;
}
// ---------------------------------------------------------------------------
void ConnectorNode::setNumber (int value) {
  Q_D (ConnectorNode);
  d->number = value;
}
// ---------------------------------------------------------------------------
int ConnectorNode::rows() const {
  Q_D (const ConnectorNode);
  return d->rows;
}
// ---------------------------------------------------------------------------
void ConnectorNode::setRows (int value) {
  Q_D (ConnectorNode);
  d->rows = value;
}
// ---------------------------------------------------------------------------
ConnectorFamily & ConnectorNode::family() {
  Q_D (ConnectorNode);
  return d->family;
}
