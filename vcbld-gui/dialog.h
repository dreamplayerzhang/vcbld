#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <string>

#include "predefs.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  ~Dialog();
  BinType binType() const;

private slots:
  void on_pushButton_clicked();
  void on_pushButton_2_clicked();
  void on_pushButton_3_clicked();

private:
  Ui::Dialog *ui;
  BinType _binType = BinType::Application;
};

#endif // DIALOG_H
