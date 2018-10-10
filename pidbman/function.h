#ifndef FUNCTION_H
#define FUNCTION_H

#include "property.h"

class QString;
class Node;
class FunctionNodePrivate;
class FunctionNode : public Node {
  public:

    FunctionNode (int pinModeId, int pinNameId, Node * parent);
    virtual ~FunctionNode();

    virtual QString name() const;

    Property & pinName();
    Property & pinMode();
    const Property & pinName() const;
    const Property & pinMode() const;

  protected:
    FunctionNode (FunctionNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (FunctionNode);
    Q_DISABLE_COPY (FunctionNode);
};

#endif
