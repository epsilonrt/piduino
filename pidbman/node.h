#ifndef NODE_H
#define NODE_H

#include "property.h"

class NodePrivate;
class Node {
  public:

    Node (Property * data, Node * parent); // property
    Node (QSqlDatabase & database); // root
    virtual ~Node();

    Node * parent() const;
    Node * root() const;

    const QList<Node *> & children() const;
    void append (Node * child);

    bool isFolder() const;
    Property * data() const;

    virtual QString name() const;
    virtual QSqlDatabase & database() const;
    virtual void childrenFromDatabase();
    void clearChildren();

  protected:
    Node (NodePrivate &dd);
    const QScopedPointer<NodePrivate> d_ptr;

  private:
    Q_DECLARE_PRIVATE (Node);
    Q_DISABLE_COPY (Node);
};
#endif
