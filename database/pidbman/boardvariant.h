#ifndef BOARDVARIANT_H
#define BOARDVARIANT_H

#include "property.h"

class QString;
class Node;
class BoardVariantNodePrivate;
class BoardVariantNode : public Node {
  public:

    BoardVariantNode (int id, const QString & name, Node * parent);
    virtual ~BoardVariantNode();

    int modelId() const;

    const QString & tag() const;
    void setTag (const QString & str);
    int revision() const;
    void setRevision (int value);
    int ram() const;
    void setRam (int value);
    const QString & pcbRevision() const;
    void setPcbRevision (const QString & str);
    
    int gpioId() const;
    void setGpioId (int value);
    
    Property & manufacturer();

  protected:
    BoardVariantNode (BoardVariantNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (BoardVariantNode);
    Q_DISABLE_COPY (BoardVariantNode);
};
#endif
