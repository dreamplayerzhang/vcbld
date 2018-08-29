#include "setupdialog.h"
#include "ui_setupdialog.h"

#include <iostream>
#include <QDesktopWidget>

SetupDialog::SetupDialog(const QString &msg, std::vector<fs::path> &vector,
                         QWidget *parent)
    : QDialog(parent), ui(new Ui::SetupDialog), _msg(msg), _vector(vector) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 120);
  setFixedSize(size());
  QString labelText = "Please choose a";
  labelText += msg;
  ui->label->setText(labelText);

  for (std::vector<fs::path>::iterator it = vector.begin(); it != vector.end();
       ++it) {
    ui->listWidget->addItem(QString::fromStdString(it->string()));
  }
}

SetupDialog::~SetupDialog() { delete ui; }

int SetupDialog::choice() { return _choice; }

void SetupDialog::on_listWidget_doubleClicked(const QModelIndex &index) {
  _choice = index.row();
  close();
}
