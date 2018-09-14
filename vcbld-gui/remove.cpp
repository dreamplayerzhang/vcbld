#include "qpch.h"

using namespace vcbld;

Remove::Remove(const std::string &param, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Remove), _param(param) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 200);
  setFixedSize(size());

  QObject::connect(this, SIGNAL(outputChanged(const QString &)), parent,
                   SLOT(on_outputChanged(const QString &)));
  QObject::connect(this, SIGNAL(vcpkgCmnd(const std::string &)), parent,
                   SLOT(on_vcpkgCmnd(const std::string &)));

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
  if (_param == "remove") {
    _output = Helper::execVec(std::bind(&args::remove, v), v);
  } else {
    emit vcpkgCmnd(pkgClass.vcpkgDirPath() + "/" + "vcpkg" + " remove " +
                   pkgName.toStdString());
    _output = Helper::execVec(std::bind(&args::remove, v), v);
  }
  emit outputChanged(_output);
  item->setHidden(true);
}
