#include "node_p.h"

// ---------------------------------------------------------------------------
//                          Abstract Class Node
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
NodePrivate::NodePrivate (Property * d, Node * p, Node * q) :
  parent (p), q_ptr (q), data (d), database (parent->database()) {

  findRoot ();
}
// ---------------------------------------------------------------------------
NodePrivate::NodePrivate (QSqlDatabase & db, Node * q) :
  parent (0), root (q), q_ptr (q), data (0), database (db) {
}
// ---------------------------------------------------------------------------
NodePrivate::~NodePrivate() {
  qDeleteAll (children);
  delete data;
}
// ---------------------------------------------------------------------------
void NodePrivate::findRoot () {
  root = q_ptr;

  while (root->parent()) {
    root = root->parent();
  }
}
// ---------------------------------------------------------------------------
Node::Node (NodePrivate &dd) : d_ptr (&dd) {
}
// ---------------------------------------------------------------------------
Node::Node (Property * data, Node * parent) :
  d_ptr (new NodePrivate (data, parent, this))  {

}
// ---------------------------------------------------------------------------
Node::Node (QSqlDatabase & database) :
  d_ptr (new NodePrivate (database, this))  {
}
// ---------------------------------------------------------------------------
Node::~Node() {

}
// ---------------------------------------------------------------------------
bool Node::isFolder() const {

  return d_ptr->data == 0;
}
// ---------------------------------------------------------------------------
Property * Node::data() const {

  return d_ptr->data;
}
// ---------------------------------------------------------------------------
QString Node::name() const {
  if (data()) {
    return data()->name();
  }
  return QString();
}
// ---------------------------------------------------------------------------
Node * Node::parent() const {

  return d_ptr->parent;
}
// ---------------------------------------------------------------------------
const QList<Node *> & Node::children() const {

  return d_ptr->children;
}
// ---------------------------------------------------------------------------
Node * Node::root() const {

  return d_ptr->root;
}
// ---------------------------------------------------------------------------
void Node::clearChildren() {
  Q_D (Node);

  if (!d->children.isEmpty()) {
    qDeleteAll (d->children);
    d->children.clear();
  }
}
// ---------------------------------------------------------------------------
void Node::append (Node * child) {

  d_ptr->children.append (child);
}
// ---------------------------------------------------------------------------
QSqlDatabase & Node::database() const {

  return  d_ptr->database;
}
// ---------------------------------------------------------------------------
void Node::childrenFromDatabase() {}
