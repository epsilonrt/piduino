TEMPLATE       = app
CONFIG += debug
QT           += sql
SOURCES     = \
             socfamily.cpp\
             soc.cpp\
             pin.cpp\
             manufacturer.cpp\
             function.cpp\
             main.cpp\
             boardmodel.cpp\
             gpio.cpp\
             treemodel.cpp\
             database.cpp\
             actionbutton.cpp\
             mainwindow.cpp\
             boardfamily.cpp\
             boardvariant.cpp\
             node.cpp\
             connector.cpp\
             folder.cpp\

HEADERS     = \
             node_p.h\
             pin.h\
             folder.h\
             boardfamily.h\
             boardvariant.h\
             database.h\
             function.h\
             mainwindow.h\
             soc.h\
             gpio.h\
             treemodel.h\
             node.h\
             connector.h\
             socfamily.h\
             actionbutton.h\
             boardmodel.h\


RESOURCES   = \
             pidbman.qrc\
