#ifndef BOARDMODEL_H
#define BOARDMODEL_H

class Node;
class QString;
class BoardModelNodePrivate;
class BoardModelNode : public Node {
  public:

    BoardModelNode (int id, const QString & name, Node * parent);
    virtual ~BoardModelNode();
    virtual void childrenFromDatabase();

    int familyId() const;

    int socId() const;
    void setSocId (int soc);

  protected:
    BoardModelNode (BoardModelNodePrivate &dd);

  private:
    Q_DECLARE_PRIVATE (BoardModelNode);
    Q_DISABLE_COPY (BoardModelNode);
};
#endif
