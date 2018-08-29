#ifndef ADD_H
#define ADD_H

#include "qpch.h"

namespace Ui {
class Add;
}

class Add : public QMainWindow {
  Q_OBJECT

public:
  explicit Add(QWidget *parent = nullptr);
  ~Add();

private slots:
  void on_pushButton_clicked();

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

  void on_lineEdit_textChanged(const QString &arg1);

private:
  vcbld::PkgClass pkgClass;
  Ui::Add *ui;
};

#endif // ADD_H
