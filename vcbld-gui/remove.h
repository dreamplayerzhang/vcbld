#ifndef REMOVE_H
#define REMOVE_H

#include "qpch.h"

namespace Ui {
class Remove;
}

class Remove : public QMainWindow {
  Q_OBJECT

public:
  explicit Remove(QWidget *parent = nullptr);
  ~Remove();

private slots:
  void on_cancelButton_clicked();

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
  vcbld::PkgClass pkgClass;
  Ui::Remove *ui;
};

#endif // REMOVE_H
