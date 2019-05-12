#include "dialog.h"
#include "qpch.h"
#include "ui_dialog.h"

using namespace vcbld;

Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 200);
  setFixedSize(size());
}

Dialog::~Dialog() { delete ui; }

BinType Dialog::binType() const { return _binType; }

void Dialog::on_pushButton_clicked() {
  _binType = BinType::Application;
  close();
}

void Dialog::on_pushButton_2_clicked() {
  _binType = BinType::DynamicLibrary;
  close();
}

void Dialog::on_pushButton_3_clicked() {
  _binType = BinType::StaticLibrary;
  close();
}
