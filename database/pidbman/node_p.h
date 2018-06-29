#ifndef NODE_PRIVATE_H
#define NODE_PRIVATE_H

#include <QtCore>
#include <QtSql>
#include "database.h"

class NodePrivate {
  public:
    NodePrivate (Node::Type t, Node * p) :
      type (t), id (-1), parent (p), q_ptr(0), is_folder (false), root(0) {}
    Node::Type type;
    int id;
    Node * parent;
    Node * q_ptr;
    bool is_folder;
    Database * root;
    QString name;
    QList<Node *> children;

    void setRoot(Node * q);
    Q_DECLARE_PUBLIC (Node);
};
#endif
