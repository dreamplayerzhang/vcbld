#ifndef ADD_H
#define ADD_H

#include <QListWidgetItem>
#include <QMainWindow>

#include "pkg.h"

namespace Ui {
class Add;
}

class Add : public QMainWindow {
  Q_OBJECT

public:
  explicit Add(QWidget *parent = nullptr);
  ~Add();
  QString output() const;

private slots:
  void on_pushButton_clicked();

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

  void on_lineEdit_textChanged(const QString &arg1);

signals:
    void outputChanged(const QString &);

private:
  vcbld::PkgClass pkgClass;
  Ui::Add *ui;
  QString _output;
};

#endif // ADD_H
