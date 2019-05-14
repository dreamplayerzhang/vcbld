#include "add.h"
#include "helper.h"
#include "qpch.h"
#include "ui_add.h"

using namespace vcbld;

Add::Add(const std::string &param, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::Add), _param(param) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 200);
  setFixedSize(size());

  QObject::connect(this, SIGNAL(outputChanged(const QString &)), parent,
                   SLOT(on_outputChanged(const QString &)));
  QObject::connect(this, SIGNAL(vcpkgCmnd(const std::string &)), parent,
                   SLOT(on_vcpkgCmnd(const std::string &)));

  std::string vcpkgDirPath = pkgClass.vcpkgDirPath();
  if (_param == "add") {
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += pkgClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";
  } else if (_param == "install") {
    vcpkgDirPath += "/";
    vcpkgDirPath += "ports";
  } else {
    QMessageBox msgBox;
    msgBox.setText("Unknown parameter");
    msgBox.exec();
  }

  _dirPath = std::move(vcpkgDirPath);

  try {
    std::set<fs::directory_entry> dirEntry;
    if (fs::is_directory(_dirPath)) {
		std::copy(fs::directory_iterator(_dirPath), fs::directory_iterator(),
			std::inserter(dirEntry, dirEntry.begin()));
      for (auto &it : dirEntry) {
        if ((it.path().filename().string()).at(0) != '.') {
          ui->listWidget->addItem(
              QString::fromStdString(it.path().filename().string()));
        }
      }
    } else {
      // msgbox?
    }
  } catch (...) {
    // fail quietly
  }
}

Add::~Add() { delete ui; }

QString Add::output() const { return _output; }

void Add::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
  QString pkgName = item->text();
  std::vector<std::string> v;
  v.push_back(pkgName.toStdString());
  if (_param == "add") {
    _output = Helper::exec([&]() { args::add(v); });
  } else {
    emit vcpkgCmnd(pkgClass.vcpkgDirPath() + "/" + "vcpkg" + " install " +
                   pkgName.toStdString() + ':' + pkgClass.architecture());
    _output = Helper::exec([&]() { args::add(v); });
  }
  emit outputChanged(_output);
}

void Add::on_lineEdit_textChanged(const QString &arg1) {
  ui->listWidget->clear();
  try {
    std::set<fs::directory_entry> dirEntry;

    if (fs::is_directory(_dirPath)) {
      std::copy(fs::directory_iterator(_dirPath), fs::directory_iterator(),
                std::inserter(dirEntry, dirEntry.begin()));
      for (auto &it : dirEntry) {
        if ((it.path().filename().string()).at(0) != '.') {
          if ((it.path().filename().string()).find(arg1.toStdString()) !=
              std::string::npos) {
            ui->listWidget->addItem(
                QString::fromStdString(it.path().filename().string()));
          }
        }
      }
    } else {
      // msgbox
    }
  } catch (...) {
    // fail quietly
  }
}
