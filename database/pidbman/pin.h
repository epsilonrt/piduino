#ifndef PIN_H
#define PIN_H

#include "property.h"

class QString;
class Node;
class PinNodePrivate;
class PinNode : public Node {
  public:

    PinNode (int id, Node * parent);
    virtual ~PinNode();
    virtual void childrenFromDatabase();

    virtual QString name() const;
    int logicalNumber() const;
    int socNumber() const;
    int systemNumber() const;
    int row() const;
    int column() const;
    Property & pinType();
    
    void setLogicalNumber (int value);
    void setSocNumber (int value);
    void setSystemNumber (int value);
    void setRow (int value);
    void setColumn (int value);

  protected:
    PinNode (PinNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (PinNode);
    Q_DISABLE_COPY (PinNode);
};
#endif
