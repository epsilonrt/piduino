#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "connectorfamily.h"

class Node;
class ConnectorNodePrivate;
class ConnectorNode : public Node {
  public:

    ConnectorNode (int id, int number, Node * parent);
    virtual ~ConnectorNode();
    virtual void childrenFromDatabase();

    int gpioId() const;
    
    int number() const;
    void setNumber(int n);
    
    int rows() const;
    void setRows(int n);
    
    ConnectorFamily & family();

  protected:
    ConnectorNode (ConnectorNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (ConnectorNode);
    Q_DISABLE_COPY (ConnectorNode);
};
#endif
