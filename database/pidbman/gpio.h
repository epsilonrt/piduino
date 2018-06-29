#ifndef GPIO_H
#define GPIO_H

class QString;
class Node;
class GpioNode : public Node {
  public:

    GpioNode (int id, const QString & name, Node * parent);
    virtual void childrenFromDatabase();

    int variantId() const;
};
#endif
