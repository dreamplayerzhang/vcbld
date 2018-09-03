#include "qpch.h"

using namespace vcbld;

Add::Add(QWidget *parent) : QMainWindow(parent), ui(new Ui::Add) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 200);
  setFixedSize(size());

  QObject::connect(this, SIGNAL(outputChanged(const QString &)), parent,
                   SLOT(on_outputChanged(const QString &)));

  try {
    std::vector<fs::directory_entry> dirEntry;
    std::string vcpkgDirPath = pkgClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += pkgClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath))) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(dirEntry));
      std::sort(dirEntry.begin(), dirEntry.end());
      dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                     dirEntry.end());
      for (std::vector<fs::directory_entry>::const_iterator it =
               dirEntry.begin();
           it != dirEntry.end(); ++it) {
        if ((it->path().filename().string()).at(0) != '.') {
          ui->listWidget->addItem(
              QString::fromStdString(it->path().filename().string()));
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

void Add::on_pushButton_clicked() { close(); }

void Add::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
  QString pkgName = item->text();
  std::vector<std::string> v;
  v.push_back(pkgName.toStdString());
  _output = Helper::execVec(std::bind(&args::add, v), v);
  emit outputChanged(_output);
}

void Add::on_lineEdit_textChanged(const QString &arg1) {
  ui->listWidget->clear();
  try {
    std::vector<fs::directory_entry> dirEntry;

    std::string vcpkgDirPath = pkgClass.vcpkgDirPath();
    vcpkgDirPath += "/";
    vcpkgDirPath += "installed";
    vcpkgDirPath += "/";
    vcpkgDirPath += pkgClass.architecture();
    vcpkgDirPath += "/";
    vcpkgDirPath += "share";

    if (fs::is_directory(static_cast<fs::path>(vcpkgDirPath))) {
      std::copy(fs::directory_iterator(vcpkgDirPath), fs::directory_iterator(),
                back_inserter(dirEntry));
      std::sort(dirEntry.begin(), dirEntry.end());
      dirEntry.erase(std::unique(dirEntry.begin(), dirEntry.end()),
                     dirEntry.end());
      for (std::vector<fs::directory_entry>::const_iterator it =
               dirEntry.begin();
           it != dirEntry.end(); ++it) {
        if ((it->path().filename().string()).at(0) != '.') {
          if ((it->path().filename().string()).find(arg1.toStdString()) !=
              std::string::npos) {
            ui->listWidget->addItem(
                QString::fromStdString(it->path().filename().string()));
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
