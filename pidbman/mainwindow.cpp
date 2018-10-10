#include <QtGui>

#include "mainwindow.h"
#include "actionbutton.h"
#include "treemodel.h"

MainWindow::MainWindow() {

  treeView = new QTreeView;

  openAction = new QAction (tr ("&Open..."), this);
  connect (openAction, SIGNAL (triggered()),
           this, SLOT (openDbFile ()));
  ActionButton *button = new ActionButton (openAction, this);

  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget (button);
  layout->addWidget (treeView);
  layout->setSizeConstraint(QLayout::SetDefaultConstraint);
  setLayout (layout);

  setWindowTitle (tr ("Piduino Db Manager"));
}

void MainWindow::openDbFile() {
  QString fileName = QFileDialog::getOpenFileName (this,
                     tr ("Open SQLite3 Database"), ".",
                     tr ("SQLite3 files (*.db)"));
  if (!fileName.isEmpty()) {

    model = new TreeModel (fileName, this);
    if (model->isOpen()) {

      treeView->setModel (model);
      treeView->resizeColumnToContents (0);
      treeView->header()->setStretchLastSection (true);
      treeView->header()->setSortIndicator (0, Qt::AscendingOrder);
      treeView->header()->setSortIndicatorShown (true);
      treeView->header()->setClickable (true);
      treeView->header()->setResizeMode(QHeaderView::ResizeToContents);
      // treeView->setContextMenuPolicy (Qt::CustomContextMenu);
      openAction->setEnabled (false);
    }
    else {
      QMessageBox::critical (0, QObject::tr ("Database Error"),
                             model->lastError().text());
    }
  }
}
