#include "node_p.h"
#include "function.h"
#include "pin.h"

// ---------------------------------------------------------------------------
//                          Class FunctionNode
// ---------------------------------------------------------------------------
class FunctionNodePrivate : public NodePrivate {
  public:
    FunctionNodePrivate (Node * parent) :
      NodePrivate (Node::TypeFunction, parent),
      pinName (QString ("gpio_pin_name"), parent->database(), true, 0),
      pinMode (QString ("gpio_pin_mode"), parent->database(), false, 0) {}
    Property pinName;
    Property pinMode;
    Q_DECLARE_PUBLIC (FunctionNode);
};
// ---------------------------------------------------------------------------
FunctionNode::FunctionNode (FunctionNodePrivate &dd) : Node (dd) {}
// ---------------------------------------------------------------------------
FunctionNode::FunctionNode (int pinModeId, int pinNameId, Node * parent) :
  Node (* new FunctionNodePrivate (parent)) {

  pinMode().setId(pinModeId);
  pinName().setId(pinNameId);
}
// ---------------------------------------------------------------------------
FunctionNode::~FunctionNode() {}
// ---------------------------------------------------------------------------
QString FunctionNode::name() const {
  if (parent()) {

    if (parent()->children().count() > 1) { // GpioNode with alternate functions
      return QString ("%1(%2)").arg (pinName().name()).arg (pinMode().name());
    }
  }
  return pinName().name();
}
// ---------------------------------------------------------------------------
Property & FunctionNode::pinName() {
  Q_D (FunctionNode);
  return d->pinName;
}
// ---------------------------------------------------------------------------
const Property & FunctionNode::pinName() const {
  Q_D (const FunctionNode);
  return d->pinName;
}
// ---------------------------------------------------------------------------
Property & FunctionNode::pinMode() {
  Q_D (FunctionNode);
  return d->pinMode;
}
// ---------------------------------------------------------------------------
const Property & FunctionNode::pinMode() const {
  Q_D (const FunctionNode);
  return d->pinMode;
}
