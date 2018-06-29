#include "property_p.h"
#include "connectorfamily.h"

// ---------------------------------------------------------------------------
//                          Class ConnectorFamily
// ---------------------------------------------------------------------------
class ConnectorFamilyPrivate : public PropertyPrivate {
  public:
    ConnectorFamilyPrivate (QSqlDatabase & database) :
      PropertyPrivate (QString ("gpio_connector_family"), database, false), columns (0)  {
    }
    int columns;
    Q_DECLARE_PUBLIC (ConnectorFamily);
};
// ---------------------------------------------------------------------------
ConnectorFamily::ConnectorFamily (ConnectorFamilyPrivate &dd) : Property (dd) {}
// ---------------------------------------------------------------------------
ConnectorFamily::ConnectorFamily (QSqlDatabase & database, QObject * parent) :
  Property (* new ConnectorFamilyPrivate (database)) {
    
  setParent (parent);
}
// ---------------------------------------------------------------------------
ConnectorFamily::~ConnectorFamily() {}
// ---------------------------------------------------------------------------
int ConnectorFamily::columns() const {
  Q_D (const ConnectorFamily);
  return d->columns;
}
// ---------------------------------------------------------------------------
bool ConnectorFamily::readFromDatabase() {
  Q_D (ConnectorFamily);
  QSqlQuery q (database());
  
  q.prepare(QString ("SELECT name,columns FROM %1 WHERE id=?").arg (table()));
  q.addBindValue (d->id);
  q.exec();
  if (q.next()) {
    bool hasChanged = false;
    QString n = q.value (0).toString();
    int c =  q.value (1).toInt();

    if (d->name != n) {
      d->name = n;
      hasChanged = true;
    }
    if (d->columns != c) {
      d->columns = c;
      hasChanged = true;
    }
    if (hasChanged) {
      emit changed();
    }
    return true;
  }
  return false;
}
