#ifndef CONNECTORFAMILY_H
#define CONNECTORFAMILY_H

#include "property.h"

class ConnectorFamilyPrivate;
class ConnectorFamily : public Property {
  public:

    ConnectorFamily (QSqlDatabase & database, QObject * parent = 0);
    virtual ~ConnectorFamily();

    int columns() const;

  public slots:
    virtual bool readFromDatabase();

  protected:
    ConnectorFamily (ConnectorFamilyPrivate &dd);

  private:
    Q_DECLARE_PRIVATE (ConnectorFamily);
    Q_DISABLE_COPY (ConnectorFamily);
};
#endif
