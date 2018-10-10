#ifndef PROPERTY_H
#define PROPERTY_H

#include <QObject>
#include <QIcon>
#include <QList>
#include <QString>
#include <QSqlDatabase>

/*
      switch (node->type()) {
        case Property::TypeBoardFamily:
        case Property::TypeSocFamily:
          return iconProvider.icon (QFileIconProvider::Folder);
        case Property::TypeBoardModel:
          return QIcon (":/images/board.png");
        case Property::TypeBoardVariant:
          return QIcon (":/images/board_variant.png");
        case Property::TypeGpio:
          return QIcon (":/images/gpio.png");
        case Property::TypeConnector:
          return QIcon (":/images/connector.png");
        case Property::TypePin:
          return QIcon (":/images/pin.png");
        case Property::TypeSoc:
          return QIcon (":/images/soc.png");
        case Property::TypeFunction:
          return QIcon (":/images/function.png");
        case Property::TypeManufacturer:
          return QIcon (":/images/manufacturer.png");
        default:
          break;
      }
 */
class Node;
class PropertyPrivate;
class Property : public QObject {
    Q_OBJECT
  public:
    enum Type { TypeBoardFamily, TypeBoardModel, TypeBoardVariant,
                TypeGpio, TypeConnectorFamily, TypeConnector,
                TypePin, TypePinType, TypePinMode, TypePinName, TypeFunction,
                TypeSocFamily, TypeSoc,
                TypeManufacturer, TypeArch,
                TypeNone = -1
              };

    Property (Type type, Node * parent);
    virtual ~Property();

    virtual bool isWritable() const;
    virtual Type type() const;

    virtual int id() const;
    virtual void setId (int id);
    virtual bool exists() const;
    virtual QList<int> identifiers() const;
    virtual bool exists (int id) const;

    virtual bool hasName() const;
    virtual QString name() const;
    virtual void setName (const QString & name);
    virtual QList<QString> names() const;
    virtual bool exists (const QString & name) const;

    virtual QIcon icon() const;
    virtual QString toolTip() const;

    QSqlDatabase & database() const;
    QString table() const;

  public slots:
    virtual bool readFromDatabase();
    virtual bool writeToDatabase();
    virtual bool deleteToDatabase();

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
