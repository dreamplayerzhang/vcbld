#include "remove.h"
#include "ui_remove.h"

using namespace vcbld;

Remove::Remove(QWidget *parent) : QMainWindow(parent), ui(new Ui::Remove) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 120);
  setFixedSize(size());

  for (std::vector<std::string>::iterator it = pkgClass.packageNames().begin();
       it != pkgClass.packageNames().end(); ++it) {
    ui->listWidget->addItem(QString::fromStdString(*it));
  }
}

Remove::~Remove() { delete ui; }

void Remove::on_cancelButton_clicked() { this->close(); }

void Remove::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
  QString pkgName = item->text();
  std::vector<std::string> v;
  v.push_back(pkgName.toStdString());
  args::remove(v);
  ui->listWidget->clear();
  for (std::vector<std::string>::iterator it = pkgClass.packageNames().begin();
       it != pkgClass.packageNames().end(); ++it) {
    ui->listWidget->addItem(QString::fromStdString(*it));
  }
}
