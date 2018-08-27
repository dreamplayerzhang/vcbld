#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QString>
#include <QWindow>

#include "add.h"
#include "args.h"
#include "dialog.h"
#include "init.h"
#include "remove.h"
#include "vcbld.h"

namespace fs = std::experimental::filesystem;

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
  QDir::setCurrent(_dirName);
  if (_dirName != "") {
    Init init(_dirName.toStdString());
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
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    fs::path path = _dirName.toStdString();
    QString display = QString::fromStdString(path.filename().string());
    ui->label_3->setText(display);
    ui->label_3->setText(_dirName);
    if (fs::exists("vcbld.json")) enableMenus();
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
    //    ConfClass confClass;
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      args::run("debug");
      // QProcess process;
      // process.start(QString::fromStdString(confClass.outputDirectory() + "/"
      // + "debug" + "/" + confClass.binaryName())); process.waitForFinished();
      // QString output = process.readAllStandardOutput();
    } else {
      args::run("release");
      // QProcess process;
      // process.start(QString::fromStdString(confClass.outputDirectory() + "/"
      // + "release" + "/" + confClass.binaryName()));
      // process.waitForFinished();
      // QString output = process.readAllStandardOutput();
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
      args::run("debug");
    } else {
      args::build("release");
      args::run("release");
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

void MainWindow::on_actionList_3_triggered() { args::list(); }


void MainWindow::setup(Init &init)
{
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
          SetupDialog *setupCompiler = new SetupDialog(" C compiler", init.cCompilers(), this);
          setupCompiler->exec();
          try {
            init.setCompiler(setupCompiler->choice());
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
            SetupDialog *setupCppCompiler = new SetupDialog(" C++ compiler", init.cppCompilers(), this);
            setupCppCompiler->exec();
            try {
              init.setCppCompiler(setupCppCompiler->choice());
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
            SetupDialog *setupCmakePath = new SetupDialog(" cmake executable", init.cmakePaths(), this);
            setupCmakePath->exec();
            try {
              init.setCmake(setupCmakePath->choice());
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
            SetupDialog *setupMakePath = new SetupDialog(" make executable", init.makePaths(), this);
            setupMakePath->exec();
            try {
              init.setMake(setupMakePath->choice());
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
            SetupDialog *setupArchiverPath = new SetupDialog("n archiver", init.archiverPaths(), this);
            setupArchiverPath->exec();
            try {
              init.setArchiver(setupArchiverPath->choice());
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
            SetupDialog *setupVcpkg = new SetupDialog(" vcpkg instance", init.vcpkgPaths(), this);
            setupVcpkg->exec();
            try {
              init.setVcpkg(setupVcpkg->choice());
            } catch (...) {
              init.setVcpkg(1);
            }
            delete setupVcpkg;
        }
    }
}
