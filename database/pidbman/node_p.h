#ifndef NODE_PRIVATE_H
#define NODE_PRIVATE_H

#include <QtCore>
#include <QtSql>
#include "node.h"

class Property;
class NodePrivate {
  public:
    NodePrivate (Property * data, Node * parent, Node * q);
    NodePrivate (QSqlDatabase & db, Node * q);
    ~NodePrivate();

    Node * parent;
    Node * root;
    Node * q_ptr;
    Property * data;
    QSqlDatabase & database;
    QList<Node *> children;

    void findRoot ();
    Q_DECLARE_PUBLIC (Node);
};
#endif
