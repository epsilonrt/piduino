#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

class QAction;
class QTreeView;
class TreeModel;

class MainWindow : public QWidget {
    Q_OBJECT

  public:
    MainWindow();

  private slots:
    void openDbFile();

  private:
    QAction *openAction;
    TreeModel *model;
    QTreeView *treeView;
};

#endif
