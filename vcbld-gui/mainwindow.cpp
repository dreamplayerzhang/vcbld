#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QWindow>
#include <cstdlib>
#include <iostream>
#include <streambuf>

#include "add.h"
#include "args.h"
#include "dialog.h"
#include "init.h"
#include "pkg.h"
#include "remove.h"
#include "setupdialog.h"
#include "vcbld.h"

namespace fs = std::experimental::filesystem;

using namespace vcbld;

MainWindow::MainWindow(const fs::path vcbldPath, QWidget *parent)
    : _vcpkgPath(vcbldPath), init(vcbldPath), QMainWindow(parent), ui(new Ui::MainWindow) {
  clear();
  ui->setupUi(this);
  menuBar()->setNativeMenuBar(false);
  setFixedSize(size());
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 0);
  on_actionAlways_on_top_triggered(true);
  if (init.vcpkgPaths().size() == 0) {
    QMessageBox msgBox;
    msgBox.setText("vcbld couldn't locate a vcpkg directory, please choose the vcpkg directory.");
    msgBox.exec();
    QString vcpkgPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "~",
                                                   QFileDialog::ShowDirsOnly);
    _vcpkgPath = vcpkgPath.toStdString();
    init = std::move(Init(_vcpkgPath));
  }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::enableMenus() {
  if (_dirName != "")
    ui->menuBuild->setEnabled(true);
  ui->menuConfig->setEnabled(true);
  ui->menuGenerate->setEnabled(true);
  ui->menuPackages->setEnabled(true);
}

void MainWindow::on_actionNew_triggered() {
  _dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "~",
                                               QFileDialog::ShowDirsOnly);
  clear();
  QDir::setCurrent(_dirName);
  if (_dirName != "") {
    fs::path path = _dirName.toStdString();
    QString display = QString::fromStdString(path.filename().string());
    ui->label_3->setText(display);
    Dialog *dialog = new Dialog(this);
    dialog->exec();
    if (dialog->binType() != "") {
      args::New(dialog->binType());
      init.init(dialog->binType());
      enableMenus();
      setup(init);
      init.setup();
    }
  }
}

void MainWindow::on_actionOpen_triggered() {
  _dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "~",
                                               QFileDialog::ShowDirsOnly);
  clear();
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    fs::path path = _dirName.toStdString();
    QString display = QString::fromStdString(path.filename().string());
    ui->label_3->setText(display);
    if (fs::exists("vcbld.json"))
      enableMenus();
  }
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
    std::string config;

    if (ui->actionDebug->isChecked()) {
      config = "debug";
    } else {
      config = "release";
    }
    // std::string command;
    //#if defined(_WIN32) || defined(_WIN64)
    //    command = "start cmd.exe @cmd /k " + confClass.outputDirectory() + "/"
    //    +
    //              config + "/" + confClass.binaryName();
    //#elif defined(__linux__)
    //    command = "xterm -hold -e " + confClass.outputDirectory() + "/" +
    //    config +
    //              "/" + confClass.binaryName() + " &";
    //#elif defined(__APPLE__) && defined(__MACH__)
    //    command = "open -a Terminal " + confClass.outputDirectory() + "/" +
    //    config +
    //              "/" + confClass.binaryName() + " &";
    //#else
    //    command = "xterm -hold -e " + confClass.outputDirectory() + "/" +
    //    config +
    //              "/" + confClass.binaryName() + " &";
    //#endif
    QDir::setCurrent(_dirName);
    args::run(config);
    //    system(command.c_str());
  }
}

void MainWindow::on_actionRun_Cmake_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::cmake(" -DCMAKE_BUILD_TYPE=Debug ");
    } else {
      args::cmake(" -DCMAKE_BUILD_TYPE=Release ");
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
    QDir::setCurrent(_dirName);
    setup(init);
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

void MainWindow::on_actionBuild_run_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::build("debug");
      //      args::run("debug");
      on_actionRun_triggered();
    } else {
      args::build("release");
      //      args::run("release");
      on_actionRun_triggered();
    }
  }
}

void MainWindow::on_actionAdd_triggered() {
  Add *add = new Add(this);
  add->show();
  add->raise();
}

void MainWindow::on_actionRemove_2_triggered() {
  Remove *rmv = new Remove(this);
  rmv->show();
  rmv->raise();
}

void MainWindow::on_actionList_3_triggered() {
  args::list();
}

void MainWindow::setup(Init &init) {
  if (fs::exists("conf.json")) {
    QMessageBox msgBox;
    msgBox.setText("conf.json was found in the directory, would you like to "
                   "reconfigure it?");
    msgBox.addButton(tr("yes"), QMessageBox::AcceptRole);
    msgBox.addButton(tr("no"), QMessageBox::RejectRole);
    if (msgBox.exec() == QMessageBox::AcceptRole) {
      fs::remove("conf.json");
      setup(init);
    } else {
      return;
    }
  } else {
    if (init.cCompilers().size() == 0) {
      QMessageBox msgBox;
      msgBox.setText("Couldn't locate a C compiler.");
      msgBox.exec();
      init.setCompiler();
    } else if (init.cCompilers().size() == 1) {
      init.setCompiler(1);
    } else {
      SetupDialog *setupCompiler =
          new SetupDialog(" C compiler", init.cCompilers(), this);
      setupCompiler->exec();
      try {
        init.setCompiler(setupCompiler->choice() + 1);
      } catch (...) {
        init.setCompiler(1);
      }
      delete setupCompiler;
    }
    if (init.cppCompilers().size() == 0) {
      QMessageBox msgBox;
      msgBox.setText("Couldn't locate a C++ compiler.");
      msgBox.exec();
      init.setCppCompiler();
    } else if (init.cppCompilers().size() == 1) {
      init.setCppCompiler(1);
    } else {
      SetupDialog *setupCppCompiler =
          new SetupDialog(" C++ compiler", init.cppCompilers(), this);
      setupCppCompiler->exec();
      try {
        init.setCppCompiler(setupCppCompiler->choice() + 1);
      } catch (...) {
        init.setCppCompiler(1);
      }
      delete setupCppCompiler;
    }
    if (init.cmakePaths().size() == 0) {
      QMessageBox msgBox;
      msgBox.setText("Couldn't locate a cmake executable.");
      msgBox.exec();
      init.setCmake();
    } else if (init.cmakePaths().size() == 1) {
      init.setCmake(1);
    } else {
      SetupDialog *setupCmakePath =
          new SetupDialog(" cmake executable", init.cmakePaths(), this);
      setupCmakePath->exec();
      try {
        init.setCmake(setupCmakePath->choice() + 1);
      } catch (...) {
        init.setCmake(1);
      }
      delete setupCmakePath;
    }
    if (init.makePaths().size() == 0) {
      QMessageBox msgBox;
      msgBox.setText("Couldn't locate a make executable.");
      msgBox.exec();
      init.setMake();
    } else if (init.makePaths().size() == 1) {
      init.setMake(1);
    } else {
      SetupDialog *setupMakePath =
          new SetupDialog(" make executable", init.makePaths(), this);
      setupMakePath->exec();
      try {
        init.setMake(setupMakePath->choice() + 1);
      } catch (...) {
        init.setMake(1);
      }
      delete setupMakePath;
    }
    if (init.archiverPaths().size() == 0) {
      QMessageBox msgBox;
      msgBox.setText("Couldn't locate an archiver.");
      msgBox.exec();
      init.setArchiver();
    } else if (init.archiverPaths().size() == 1) {
      init.setArchiver(1);
    } else {
      SetupDialog *setupArchiverPath =
          new SetupDialog("n archiver", init.archiverPaths(), this);
      setupArchiverPath->exec();
      try {
        init.setArchiver(setupArchiverPath->choice() + 1);
      } catch (...) {
        init.setArchiver(1);
      }
      delete setupArchiverPath;
    }
    if (init.vcpkgPaths().size() == 0) {
      QMessageBox msgBox;
      msgBox.setText("Couldn't locate a vcpkg instance.");
      msgBox.exec();
      init.setVcpkg();
    } else if (init.vcpkgPaths().size() == 1) {
      init.setVcpkg(1);
    } else {
      SetupDialog *setupVcpkg =
          new SetupDialog(" vcpkg instance", init.vcpkgPaths(), this);
      setupVcpkg->exec();
      try {
        init.setVcpkg(setupVcpkg->choice() + 1);
      } catch (...) {
        init.setVcpkg(1);
      }
      delete setupVcpkg;
    }
  }
}

void MainWindow::on_actionRestore_triggered() { args::restore(); }

void MainWindow::clear() {
  #if defined (_WIN32) || defined(_WIN64)
    system ("cls");
  #else
    system ("clear");
  #endif
}
