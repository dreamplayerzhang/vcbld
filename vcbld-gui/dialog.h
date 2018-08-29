#ifndef DIALOG_H
#define DIALOG_H

#include "qpch.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog {
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = nullptr);
  ~Dialog();
  std::string binType() const;

private slots:

  void on_pushButton_clicked();

  void on_pushButton_2_clicked();

  void on_pushButton_3_clicked();

private:
  Ui::Dialog *ui;
  QString _binType;
};

#endif // DIALOG_H
