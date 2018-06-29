#include "node_p.h"
#include "pin.h"
#include "function.h"

// ---------------------------------------------------------------------------
//                          Class PinNode
// ---------------------------------------------------------------------------
class PinNodePrivate : public NodePrivate {
  public:
    PinNodePrivate (Node * parent) :
      NodePrivate (Node::TypePin, parent), logical_num (-1), mcu_num (-1),
      system_num (-1), row (-1), column (-1), 
      pinType (QString("gpio_pin_type"), parent->database(), false, 0) {}
    int logical_num;
    int mcu_num;
    int system_num;
    int row;
    int column;
    int gpio_pin_type_id;
    Property pinType;
    Q_DECLARE_PUBLIC (PinNode);
};
// ---------------------------------------------------------------------------
PinNode::PinNode (PinNodePrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
PinNode::PinNode (int i, Node * parent) :
  Node (* new PinNodePrivate (parent)) {
  QSqlQuery q (database());

  setId (i);

  q.prepare ("SELECT logical_num,mcu_num,system_num,gpio_pin_type_id "
             "FROM gpio_pin "
             "WHERE id = ?");
  q.addBindValue (i);
  q.exec();

  if (q.next()) {

    setLogicalNumber (q.value (0).toInt());
    setSocNumber (q.value (1).toInt());
    setSystemNumber (q.value (2).toInt());
    pinType().setId(q.value (3).toInt());
  }
  q.prepare ("SELECT name "
             "FROM gpio_pin_name "
             "INNER JOIN gpio_pin_has_name ON gpio_pin_name.id = gpio_pin_has_name.gpio_pin_name_id "
             "WHERE gpio_pin_has_name.gpio_pin_id = ?");
  q.addBindValue (i);
  q.exec();

  if (q.next()) {
    setName (q.value (0).toString());
  }
  childrenFromDatabase();
}
// ---------------------------------------------------------------------------
void PinNode::childrenFromDatabase() {
  QSqlQuery q (database());
  q.prepare ("SELECT gpio_pin_mode_id,gpio_pin_name_id "
             "FROM gpio_pin_has_name "
             "WHERE gpio_pin_id = ?");
  q.addBindValue (id());
  q.exec();
  clearChildren();

  int s = 0;
  while (q.next()) {
    s++;
  }
  if (s > 1) {
    q.first();
    while (q.next()) {

      int i = q.value (0).toInt();
      int n = q.value (1).toInt();
      if ((i >= 0) && (n >= 0)) {

        appendChild (new FunctionNode (i, n, this));
      }
    }
  }
}
// ---------------------------------------------------------------------------
PinNode::~PinNode() {}
// ---------------------------------------------------------------------------
QString PinNode::name() const {
  if (parent()) {
    if (parent()->type() == Node::TypeConnector) {
      return QString ("%1(%2,%3)").arg (Node::name()).arg (row()).arg (column());
    }
  }
  return Node::name();
}

// ---------------------------------------------------------------------------
int PinNode::logicalNumber() const {
  Q_D (const PinNode);
  return d->logical_num;
}
// ---------------------------------------------------------------------------
int PinNode::socNumber() const {
  Q_D (const PinNode);
  return d->mcu_num;
}
// ---------------------------------------------------------------------------
int PinNode::systemNumber() const {
  Q_D (const PinNode);
  return d->system_num;
}
// ---------------------------------------------------------------------------
int PinNode::row() const {
  Q_D (const PinNode);
  return d->row;
}
// ---------------------------------------------------------------------------
int PinNode::column() const {
  Q_D (const PinNode);
  return d->column;
}
// ---------------------------------------------------------------------------
Property & PinNode::pinType() {
  Q_D (PinNode);
  return d->pinType;
}
// ---------------------------------------------------------------------------
void PinNode::setLogicalNumber (int value) {
  Q_D (PinNode);
  d->logical_num = value;
}
// ---------------------------------------------------------------------------
void PinNode::setSocNumber (int value) {
  Q_D (PinNode);
  d->mcu_num = value;
}
// ---------------------------------------------------------------------------
void PinNode::setSystemNumber (int value) {
  Q_D (PinNode);
  d->system_num = value;
}
// ---------------------------------------------------------------------------
void PinNode::setRow (int value) {
  Q_D (PinNode);
  d->row = value;
}
// ---------------------------------------------------------------------------
void PinNode::setColumn (int value) {
  Q_D (PinNode);
  d->column = value;
}
