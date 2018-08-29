#include "add.h"
#include "ui_add.h"

namespace fs = std::experimental::filesystem;

using namespace vcbld;

Add::Add(QWidget *parent) : QMainWindow(parent), ui(new Ui::Add) {
  ui->setupUi(this);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 120);
  setFixedSize(size());

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

void Add::on_pushButton_clicked() { close(); }

void Add::on_listWidget_itemDoubleClicked(QListWidgetItem *item) {
  QString pkgName = item->text();
  std::vector<std::string> v;
  v.push_back(pkgName.toStdString());
  args::add(v);
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
