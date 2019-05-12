#include "setupdialog.h"
#include "qpch.h"
#include "ui_setupdialog.h"

using namespace vcbld;

SetupDialog::SetupDialog(const QString &msg, std::vector<fs::path> &vector,
                         QWidget *parent)
    : QDialog(parent), ui(new Ui::SetupDialog), _msg(msg), _vector(vector) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 200);
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

void SetupDialog::resizeEvent(QResizeEvent *) {
  ui->listWidget->resize(width(), height() - 60);
}
