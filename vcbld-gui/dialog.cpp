#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 120);
  setFixedSize(size());
}

Dialog::~Dialog() { delete ui; }

std::string Dialog::binType() const { return _binType.toStdString(); }

void Dialog::on_pushButton_clicked() {
  _binType = "app";
  close();
}

void Dialog::on_pushButton_2_clicked() {
  _binType = "dylib";
  close();
}

void Dialog::on_pushButton_3_clicked() {
  _binType = "statlib";
  close();
}
