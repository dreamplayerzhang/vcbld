#include "qpch.h"

using namespace vcbld;

MainWindow::MainWindow(const fs::path vcbldPath, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), init(vcbldPath),
      _vcpkgPath(vcbldPath), _vcbldPath(vcbldPath) {
  clear();
  ui->setupUi(this);
  menuBar()->setNativeMenuBar(false);
  setFixedSize(size());
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 0);
  ui->plainTextEdit->setReadOnly(true);
  ui->plainTextEdit->zoomOut();

  if (init.vcpkgPaths().size() == 0) {
    QMessageBox msgBox;
    msgBox.setText("vcbld couldn't locate a vcpkg directory, please choose the "
                   "vcpkg directory.");
    msgBox.exec();
    QString vcpkgPath = QFileDialog::getExistingDirectory(
        this, tr("Open Directory"), QString::fromLatin1(std::getenv("HOME")),
        QFileDialog::ShowDirsOnly);
    _vcpkgPath = vcpkgPath.toStdString();
    init = Init(_vcpkgPath);
  }
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_outputChanged(const QString &output) {
  ui->plainTextEdit->appendPlainText(output);
}

void MainWindow::enableMenus() {
  if (_dirName != "")
    ui->menuBuild->setEnabled(true);
  ui->menuConfig->setEnabled(true);
  ui->menuGenerate->setEnabled(true);
  ui->menuPackages->setEnabled(true);
}

void MainWindow::on_actionNew_triggered() {
  _dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString::fromStdString(std::getenv("HOME")),
                                               QFileDialog::ShowDirsOnly);
  clear();
  QDir::setCurrent(_dirName);
  if (_dirName != "") {
    fs::path path = _dirName.toStdString();
    QString display = QString::fromStdString(path.filename().string());
    setWindowTitle("vcbld-gui\t" + display);
    Dialog *dialog = new Dialog(this);
    dialog->exec();
    if (dialog->binType() != "") {
      ui->plainTextEdit->appendPlainText(Helper::execArgs(
          std::bind(&args::New, dialog->binType()), dialog->binType()));
      init.init(dialog->binType());
      enableMenus();
      setup(init);
      init.setup();
    }
  }
}

void MainWindow::on_actionOpen_triggered() {
  _dirName = QFileDialog::getExistingDirectory(this, tr("Open Directory"), QString::fromStdString(std::getenv("HOME")),
                                               QFileDialog::ShowDirsOnly);
  clear();
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    fs::path path = _dirName.toStdString();
    QString display = QString::fromStdString(path.filename().string());
    setWindowTitle("vcbld-gui\t" + display);
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
      Builder builder("debug");
      ui->plainTextEdit->appendPlainText("Compiling in debug...");
      compile(builder);
      if (builder.binaryType() == "app") {
        ui->plainTextEdit->appendPlainText("Linking...");
        appLink(builder);
        builder.copy();
      } else if (builder.binaryType() == "dynamicLibrary") {
        ui->plainTextEdit->appendPlainText("Linking...");
        libLink(builder);
        builder.copy();
      } else if (builder.binaryType() == "staticLibrary") {
        ui->plainTextEdit->appendPlainText("Archiving...");
        archive(builder);
      } else {
        QMessageBox msgBox;
        msgBox.setText("Unknown binary typy");
        msgBox.exec();
      }
    } else {
      Builder builder("release");
      ui->plainTextEdit->appendPlainText("Compiling in release...");
      compile(builder);
      if (builder.binaryType() == "app") {
        ui->plainTextEdit->appendPlainText("Linking...");
        appLink(builder);
        builder.copy();
      } else if (builder.binaryType() == "dylib") {
        ui->plainTextEdit->appendPlainText("Linking...");
        libLink(builder);
        builder.copy();
      } else if (builder.binaryType() == "statlib") {
        ui->plainTextEdit->appendPlainText("Archiving...");
        archive(builder);
      } else {
        QMessageBox msgBox;
        msgBox.setText("Unknown binary typy");
        msgBox.exec();
      }
    }
    ui->plainTextEdit->appendPlainText("Done...");
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
    QMessageBox msgBox;
    msgBox.setText("Cleaned build output.");
    msgBox.exec();
  }
}

void MainWindow::on_actionRun_triggered() {
  if (_dirName != "") {
    ConfClass confClass;
    std::string config, command;

    if (ui->actionDebug->isChecked()) {
      config = "debug";
    } else {
      config = "release";
    }

#if defined(_WIN32) || defined(_WIN64)
    command = "start cmd.exe @cmd /k " + confClass.outputDirectory() + "/" +
              config + "/" + confClass.binaryName();
#elif defined(__linux__)
    command = "xterm -hold -e " + confClass.outputDirectory() + "/" + config +
              "/" + confClass.binaryName() + " &";
#elif defined(__APPLE__) && defined(__MACH__)
    command = "open -a Terminal " + confClass.outputDirectory() + "/" + config +
              "/" + confClass.binaryName();
#else
    command = "xterm -hold -e " + confClass.outputDirectory() + "/" + config +
              "/" + confClass.binaryName();
#endif
    QDir::setCurrent(_dirName);
    QProcess proc(this);
    proc.startDetached(QString::fromStdString(command));
  }
}

void MainWindow::on_actionRun_Cmake_triggered() {
  if (_dirName != "") {
    ConfClass confClass;
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      QProcess proc(this);
      proc.setWorkingDirectory(
          QString::fromStdString(confClass.outputDirectory()));
      proc.start(QString::fromStdString(confClass.cmakePath()),
                 QStringList() << "-DCMAKE_BUILD_TYPE=Debug"
                               << "..");
      proc.waitForFinished(-1);
      QString err = proc.readAllStandardError();
      QString out = proc.readAllStandardOutput();
      ui->plainTextEdit->appendPlainText(err);
      ui->plainTextEdit->appendPlainText(out);
    } else {
      QProcess proc(this);
      proc.setWorkingDirectory(
          QString::fromStdString(confClass.outputDirectory()));
      proc.start(QString::fromStdString(confClass.cmakePath()),
                 QStringList() << "-DCMAKE_BUILD_TYPE=Release"
                               << "..");
      proc.waitForFinished(-1);
      QString err = proc.readAllStandardError();
      QString out = proc.readAllStandardOutput();
      ui->plainTextEdit->appendPlainText(err);
      ui->plainTextEdit->appendPlainText(out);
    }
  }
}

void MainWindow::on_actionRun_make_triggered() {
  if (_dirName != "") {
    ConfClass confClass;
    QDir::setCurrent(_dirName);
    QProcess proc(this);
    proc.setWorkingDirectory(
        QString::fromStdString(confClass.outputDirectory()));
    proc.start(QString::fromStdString(confClass.makePath()));
    proc.waitForFinished(-1);
    QString err = proc.readAllStandardError();
    QString out = proc.readAllStandardOutput();
    ui->plainTextEdit->appendPlainText(err);
    ui->plainTextEdit->appendPlainText(out);
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
    setWindowTitle("vcbld-gui");
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
    ui->plainTextEdit->appendPlainText(
        Helper::exec(std::bind(&args::generate)));
  }
}

void MainWindow::on_actionBuild_run_triggered() {
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    if (ui->actionDebug->isChecked()) {
      on_actionBuild_triggered();
      on_actionRun_triggered();
    } else {
      on_actionBuild_triggered();
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
  ui->plainTextEdit->appendPlainText(Helper::exec(std::bind(&args::list)));
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

void MainWindow::on_actionClear_output_2_triggered() {
  ui->plainTextEdit->clear();
}

void MainWindow::on_actionRestore_triggered() {
  ui->plainTextEdit->appendPlainText(Helper::exec(std::bind(&args::restore)));
}

void MainWindow::clear() {
  int systemRet;
#if defined(_WIN32) || defined(_WIN64)
  systemRet = system("cls");
#else
  systemRet = system("clear");
#endif
  if (systemRet == -1) {
    std::cout << "Error clearing the terminal." << std::endl;
  }
}

void MainWindow::compile(Builder &builder) {
  if (ui->actionDebug->isChecked()) {
    std::ostringstream command;
    command << "\"" << builder.compilerPath() << "\" " << builder.headerPaths()
            << " " << builder.compilerFlags() << " "
            << builder.compilerDefines() << " -g "
            << "-std=" << builder.language() << builder.standard() << " -c "
            << builder.sourceFiles();
    QProcess proc(this);
    proc.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/debug"));
    proc.start(QString::fromStdString(command.str()));
    proc.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(proc.readAllStandardError());
    ui->plainTextEdit->appendPlainText(proc.readAllStandardOutput());
  } else {
    std::ostringstream command;
    command << "\"" << builder.compilerPath() << "\" " << builder.headerPaths()
            << " " << builder.compilerFlags() << " "
            << builder.compilerDefines() << " "
            << "-std=" << builder.language() << builder.standard() << " -c "
            << builder.sourceFiles();
    QProcess proc(this);
    proc.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/release"));
    proc.start(QString::fromStdString(command.str()));
    proc.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(proc.readAllStandardError());
    ui->plainTextEdit->appendPlainText(proc.readAllStandardOutput());
  }
}

void MainWindow::appLink(Builder &builder) {
  if (ui->actionDebug->isChecked()) {
    std::ostringstream command;
    command << "\"" << builder.compilerPath() << "\" -o "
            << builder.binaryName() << " "
            << builder.objPath(builder.outputDirectory() + "/debug") << " "
            << builder.rlsLibPaths() << " " << builder.linkerFlags();
    QProcess procLink(this);
    procLink.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/debug"));
    procLink.start(QString::fromStdString(command.str()));
    procLink.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardError());
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardOutput());
  } else {
    std::ostringstream command;
    command << "\"" << builder.compilerPath() << "\" -o "
            << builder.binaryName() << " "
            << builder.objPath(builder.outputDirectory() + "/release") << " "
            << builder.rlsLibPaths() << " " << builder.linkerFlags();
    QProcess procLink(this);
    procLink.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/release"));
    procLink.start(QString::fromStdString(command.str()));
    procLink.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardError());
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardOutput());
  }
}

void MainWindow::libLink(Builder &builder) {
  std::string dylibArg, dylibExt;
  if (builder.compilerPath().find("clang") != std::string::npos) {
    dylibArg = " -dynamiclib ";
    dylibExt = ".dylib";
  } else {
    dylibArg = " -shared ";
    dylibExt = ".so";
  }

  if (ui->actionDebug->isChecked()) {
    std::ostringstream command;
    command << "\"" << builder.compilerPath() << "\"" << dylibArg << " -o "
            << builder.binaryName() << dylibExt << " "
            << builder.objPath(builder.outputDirectory() + "/debug") << " "
            << builder.dbgLibPaths() << " " << builder.linkerFlags();
    QProcess procLink(this);
    procLink.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/debug"));
    procLink.start(QString::fromStdString(command.str()));
    procLink.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardError());
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardOutput());
  } else {
    std::ostringstream command;
    command << "\"" << builder.compilerPath() << "\"" << dylibArg << " -o "
            << builder.binaryName() << dylibExt << " "
            << builder.objPath(builder.outputDirectory() + "/release") << " "
            << builder.rlsLibPaths() << " " << builder.linkerFlags();
    QProcess procLink(this);
    procLink.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/release"));
    procLink.start(QString::fromStdString(command.str()));
    procLink.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardError());
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardOutput());
  }
}

void MainWindow::archive(Builder &builder) {
  if (ui->actionDebug->isChecked()) {
    std::ostringstream command;
    command << "\"" << builder.archiverPath() << "\" rcs "
            << builder.binaryName() << ".a"
            << " " << builder.objPath(builder.outputDirectory() + "/debug");
    QProcess procLink(this);
    procLink.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/debug"));
    procLink.start(QString::fromStdString(command.str()));
    procLink.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardError());
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardOutput());
  } else {
    std::ostringstream command;
    command << "\"" << builder.archiverPath() << "\" rcs "
            << builder.binaryName() << ".a"
            << " " << builder.objPath(builder.outputDirectory() + "/release");
    QProcess procLink(this);
    procLink.setWorkingDirectory(
        QString::fromStdString(builder.outputDirectory() + "/release"));
    procLink.start(QString::fromStdString(command.str()));
    procLink.waitForFinished(-1);
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardError());
    ui->plainTextEdit->appendPlainText(procLink.readAllStandardOutput());
  }
}
