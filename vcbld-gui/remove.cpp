#include "qpch.h"

using namespace vcbld;

Remove::Remove(QWidget *parent) : QMainWindow(parent), ui(new Ui::Remove) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 200);
  setFixedSize(size());

  QObject::connect(this, SIGNAL(outputChanged(const QString &)), parent,
                   SLOT(on_outputChanged(const QString &)));

  for (std::vector<std::string>::iterator it = pkgClass.packageNames().begin();
       it != pkgClass.packageNames().end(); ++it) {
    ui->listWidget->addItem(QString::fromStdString(*it));
  }
}

Remove::~Remove() { delete ui; }

QString Remove::output() const { return _output; }

void Remove::on_cancelButton_clicked() { this->close(); }

void Remove::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
  QString pkgName = item->text();
  std::vector<std::string> v;
  v.push_back(pkgName.toStdString());
  _output = Helper::execVec(std::bind(&args::remove, v), v);
  emit outputChanged(_output);
  item->setHidden(true);
}
