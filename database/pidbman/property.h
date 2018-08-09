#ifndef PROPERTY_H
#define PROPERTY_H

#include "enumstring.h"
#include <QObject>

class PropertyPrivate;
class Property : public QObject {
    Q_OBJECT
  public:

    Property (const QString & tableName, QSqlDatabase & database, bool isWritable = false, QObject * parent = 0);
    virtual ~Property();

    int id() const;
    void setId (int id);

    virtual QString name() const;
    virtual void setName (const QString & name);

    const EnumString & values() const;

    QSqlDatabase & database() const;
    QString table() const;

    bool isWritable() const;
    bool exists() const;

  public slots:
    virtual bool readFromDatabase();
    virtual bool writeToDatabase();
    virtual void valuesFromDatabase();

  signals:
    void updated();
    void changed();

  protected:
    Property (PropertyPrivate &dd);
    const QScopedPointer<PropertyPrivate> d_ptr;

  private:
    Q_DECLARE_PRIVATE (Property);
    Q_DISABLE_COPY (Property);
};
#endif
