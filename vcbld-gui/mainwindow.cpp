#include "mainwindow.h"
#include "../src/args.h"
#include "../src/init.h"
#include "../src/vcbld.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>

using namespace vcbld;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  menuBar()->setNativeMenuBar(false);
  setFixedSize(size());
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 0);
  on_actionAlways_on_top_triggered(true);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_actionNew_triggered() {
  _dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "~",
                                               QFileDialog::ShowDirsOnly);
  QDir::setCurrent(_dirName);
  if (_dirName != "") {
    Init init(_dirName.toStdString());
    ui->label_3->setText(_dirName);
    args::New("app");
    init.init("app");
    init.setCompiler();
    init.setCppCompiler();
    init.setVcpkg();
    init.setCmake();
    init.setMake();
    init.setArchiver();
    init.setup();
  }
}

void MainWindow::on_actionOpen_triggered() {
  _dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "~",
                                               QFileDialog::ShowDirsOnly);
  QDir::setCurrent(_dirName);
  ui->label_3->setText(_dirName);
}

void MainWindow::on_actionAlways_on_top_triggered(bool checked) {
#ifdef Q_OS_WIN
  // #include <windows.h>
  if (checked) {
    SetWindowPos(winId(), HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  } else {
    SetWindowPos(winId(), HWND_NOTOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  }
#else
  Qt::WindowFlags flags = windowFlags();
  if (checked) {
    setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    show();
  } else {
    setWindowFlags(flags ^
                   (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
    show();
  }
#endif
}

void MainWindow::on_actionDebug_triggered(bool checked) {
  if (checked == true) {
    ui->actionRelease->setChecked(false);
  }
}

void MainWindow::on_actionRelease_triggered(bool checked) {
  if (checked == true) {
    ui->actionDebug->setChecked(false);
  }
}

void MainWindow::on_actionBuild_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::build("debug");
    } else {
      args::build("release");
    }
  }
}

void MainWindow::on_actionClean_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::clean("debug");
    } else {
      args::clean("release");
    }
  }
}

void MainWindow::on_actionRun_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::run("debug");
    } else {
      args::run("release");
    }
  }
}

void MainWindow::on_actionRun_Cmake_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::cmake(" -DCMAKE_BUILD_TYPE=Debug ");
    } else {
      args::run(" -DCMAKE_BUILD_TYPE=Release ");
    }
  }
}

void MainWindow::on_actionRun_make_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::make(" ");
    } else {
      args::make(" ");
    }
  }
}

void MainWindow::on_actionSetup_triggered() {
  if (_dirName == "") {
    QMessageBox msgBox;
    msgBox.setText("Please choose a working directory.");
    msgBox.exec();
  } else {
    Init init(_dirName.toStdString());
    QDir::setCurrent(_dirName);
    init.init("app");
    init.setCompiler();
    init.setCppCompiler();
    init.setVcpkg();
    init.setCmake();
    init.setMake();
    init.setArchiver();
    init.setup();
  }
}

void MainWindow::on_actionAbout_triggered() {
  QMessageBox msgBox;
  msgBox.setText("vcbld-gui version " + QString::number(MAJOR_VERSION) + "." +
                 QString::number(MINOR_VERSION) + "." +
                 QString::number(PATCH_VERSION) + "\nBuilt using Qt5.");
  msgBox.exec();
}

void MainWindow::on_actionClose_triggered() {
  if (_dirName != "") {
    _dirName = "";
    ui->label_3->setText("");
    QDir::setCurrent(_dirName);
  }
}

void MainWindow::on_actionExit_triggered() { QApplication::exit(); }

void MainWindow::on_actionHelp_triggered() {
  QMessageBox msgBox;
  msgBox.setText("Please check the vcbld github repo "
                 "<a href=\"https://github.com/MoAlyousef/vcbld\"> here<a>.");
  msgBox.exec();
}

void MainWindow::on_actionIncludePath_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    args::includes();
  }
}

void MainWindow::on_actionCMakeLists_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    args::generate();
  }
}
