#include "qpch.h"

using namespace vcbld;

MainWindow::MainWindow(const fs::path vcbldPath, QWidget *parent)
    : QMainWindow(parent), proc(new QProcess(this)), ui(new Ui::MainWindow),
      init(vcbldPath), _vcpkgPath(vcbldPath), _vcbldPath(vcbldPath) {
  ui->setupUi(this);
  menuBar()->setNativeMenuBar(false);
  QDesktopWidget *desktop = QApplication::desktop();
  int screenWidth = desktop->width();
  move(screenWidth / 2 - width() / 2, 0);

  ui->plainTextEdit->setReadOnly(true);
  //  ui->plainTextEdit->zoomOut(1);
  statusLabel = new QLabel(this);
  ui->statusBar->addWidget(statusLabel, ui->statusBar->width());
  statusLabel->setText("Current directory: ");
  proc->isSequential();

  QObject::connect(proc, SIGNAL(readyReadStandardOutput()), this,
                   SLOT(local_outputChanged()));
  QObject::connect(proc, SIGNAL(readyReadStandardError()), this,
                   SLOT(local_outputChanged()));
  QObject::connect(proc, SIGNAL(finished(int)), this,
                   SLOT(on_procFinished(int)));
  QObject::connect(this, SIGNAL(outputChanged(const QString &)), this,
                   SLOT(on_outputChanged(const QString &)));

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

MainWindow::~MainWindow() {
  proc->deleteLater();
  delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
  ui->plainTextEdit->resize(width(), height() - 60);
}

void MainWindow::on_outputChanged(const QString &output) {
  ui->plainTextEdit->appendPlainText(output);
}

void MainWindow::local_outputChanged() {
  QString output = proc->readAllStandardOutput();
  QString error = proc->readAllStandardError();
  if (output != "")
    ui->plainTextEdit->appendPlainText(output);
  if (error != "")
    ui->plainTextEdit->appendPlainText(error);
}

void MainWindow::runProcess(const QString &process, const QString &dir) {
  proc->setWorkingDirectory(dir);
  proc->start(process);
  enableMenus(false);
  QEventLoop loop;
  QObject::connect(proc, SIGNAL(finished(int)), &loop, SLOT(quit()));
  loop.exec();
}

void MainWindow::on_procFinished(int) { enableMenus(true); }

void MainWindow::on_actionToolbar_triggered() {
  QFont font = ui->menuBar->font();
  font.setPointSize(font.pointSize() + 1);
  ui->menuBar->setFont(font);
  ui->menuBuild->setFont(font);
  ui->menuConfig->setFont(font);
  ui->menuGenerate->setFont(font);
  ui->menuHelp->setFont(font);
  ui->menuOpen->setFont(font);
  ui->menuPackages->setFont(font);
  ui->menuView->setFont(font);
}

void MainWindow::on_actionToolbar_2_triggered() {
  QFont font = ui->menuBar->font();
  font.setPointSize(font.pointSize() - 1);
  ui->menuBar->setFont(font);
  ui->menuBuild->setFont(font);
  ui->menuConfig->setFont(font);
  ui->menuGenerate->setFont(font);
  ui->menuHelp->setFont(font);
  ui->menuOpen->setFont(font);
  ui->menuPackages->setFont(font);
  ui->menuView->setFont(font);
}

void MainWindow::on_actionOutput_triggered() { ui->plainTextEdit->zoomIn(1); }

void MainWindow::on_actionOutput_2_triggered() {
  ui->plainTextEdit->zoomOut(1);
}

void MainWindow::enableMenus(bool val) {
  ui->menuBuild->setEnabled(val);
  ui->menuConfig->setEnabled(val);
  ui->menuGenerate->setEnabled(val);
  ui->menuPackages->setEnabled(val);
}

void MainWindow::on_actionNew_triggered() {
  _dirName = QFileDialog::getExistingDirectory(
      this, tr("Open Directory"), QString::fromLatin1(std::getenv("HOME")),
      QFileDialog::ShowDirsOnly);
  QDir::setCurrent(_dirName);
  if (_dirName != "") {
    fs::path path = _dirName.toStdString();
    QString display = QString::fromStdString(path.string());
    statusLabel->setText("Current directory: " + _dirName);
    this->setWindowTitle("vcbld-gui\t--\t" + display);
    Dialog *dialog = new Dialog(this);
    dialog->exec();
    if (dialog->binType() != "") {
      ui->plainTextEdit->appendPlainText(Helper::execArgs(
          std::bind(&args::New, dialog->binType()), dialog->binType()));
      init.init(dialog->binType());
      enableMenus(true);
      setup(init);
      init.setup();
    }
  }
}

void MainWindow::on_actionOpen_triggered() {
  _dirName = QFileDialog::getExistingDirectory(
      this, tr("Open Directory"), QString::fromLatin1(std::getenv("HOME")),
      QFileDialog::ShowDirsOnly);
  if (_dirName != "") {
    QDir::setCurrent(_dirName);
    fs::path path = _dirName.toStdString();
    QString display = QString::fromStdString(path.filename().string());
    statusLabel->setText("Current directory: " + _dirName);
    this->setWindowTitle("vcbld-gui\t--\t" + display);
    if (fs::exists("vcbld.json"))
      enableMenus(true);
  }
}

void MainWindow::on_actionAlways_on_top_triggered(bool checked) {
  Qt::WindowFlags flags = windowFlags();
  if (checked) {
    setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    show();
  } else {
    setWindowFlags(flags ^
                   (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
    show();
  }
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
      if (!fs::exists(builder.outputDirectory()))
        fs::create_directory(builder.outputDirectory());
      if (!fs::exists(builder.outputDirectory() + "/" + "debug"))
        fs::create_directory(builder.outputDirectory() + "/" + "debug");
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
      if (!fs::exists(builder.outputDirectory()))
        fs::create_directory(builder.outputDirectory());
      if (!fs::exists(builder.outputDirectory() + "/" + "release"))
        fs::create_directory(builder.outputDirectory() + "/" + "release");
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
        msgBox.setText("Unknown binary typy.");
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
    ui->plainTextEdit->appendPlainText("Build output cleaned.");
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
    command = "cmd.exe /C " +
              fs::canonical(confClass.outputDirectory()).string() + "\\" +
              config + "\\" + confClass.binaryName() + ".exe";
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

    system(command.c_str());
  }
}

void MainWindow::on_actionRun_Cmake_triggered() {
  if (_dirName != "") {

    QInputDialog inDlg;
    inDlg.setLabelText("Please enter any cmake arguments (optional):");
    QString args = inDlg.getText(this, tr("CMake Arguments"),
                                 tr("Please enter any CMake arguments:"),
                                 QLineEdit::Normal);

    ConfClass confClass;
    std::ostringstream cmakeCmnd, temp;
    std::string config;
    if (ui->actionDebug->isChecked()) {
      config = "Debug";
    } else {
      config = "Release";
    }

    if (confClass.architecture().find("windows") == std::string::npos) {
      temp << " -DCMAKE_C_COMPILER=\"" << confClass.cCompilerPath() << "\""
           << " -DCMAKE_CXX_COMPILER=\"" << confClass.cppCompilerPath() << "\""
           << " -DCMAKE_MAKE_PROGRAM=\"" << confClass.makePath() << "\" ";
    } else {
      temp << " ";
    }
    cmakeCmnd << " \"" << confClass.cmakePath() << "\""
              << " -DCMAKE_BUILD_TYPE=" << config
              << " -DCMAKE_TOOLCHAIN_FILE=\"" << confClass.vcpkgDirPath()
              << "/scripts/buildsystems/vcpkg.cmake"
              << "\"" << temp.str() << " " << args.toStdString() << " .. ";

    QDir::setCurrent(_dirName);
    runProcess(QString::fromStdString(cmakeCmnd.str()),
               QString::fromStdString(confClass.outputDirectory()));
  }
}

void MainWindow::on_actionRun_make_triggered() {
  if (_dirName != "") {
    ConfClass confClass;
    QDir::setCurrent(_dirName);
    runProcess(QString::fromStdString(confClass.makePath()),
               QString::fromStdString(confClass.outputDirectory()));
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
  msgBox.setText(
      "vcbld-gui version " + QString::number(vcbldgui_MAJOR_VERSION) + "." +
      QString::number(vcbldgui_MINOR_VERSION) + "." +
      QString::number(vcbldgui_PATCH_VERSION) + ".\nBuilt using Qt5.");
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
    ui->plainTextEdit->appendPlainText(
        Helper::exec(std::bind(&args::includes)));
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
    on_actionBuild_triggered();
    on_actionRun_triggered();
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
    runProcess(QString::fromStdString(builder.compileCommand()),
               QString::fromStdString(builder.outputDirectory() + "/debug"));
  } else {
    runProcess(QString::fromStdString(builder.compileCommand()),
               QString::fromStdString(builder.outputDirectory() + "/release"));
  }
}

void MainWindow::appLink(Builder &builder) {
  if (ui->actionDebug->isChecked()) {
    runProcess(QString::fromStdString(builder.appLinkCmnd()),
               QString::fromStdString(builder.outputDirectory() + "/debug"));
  } else {
    runProcess(QString::fromStdString(builder.appLinkCmnd()),
               QString::fromStdString(builder.outputDirectory() + "/release"));
  }
}

void MainWindow::libLink(Builder &builder) {
  if (ui->actionDebug->isChecked()) {
    runProcess(QString::fromStdString(builder.libLinkCmnd()),
               QString::fromStdString(builder.outputDirectory() + "/debug"));
  } else {
    runProcess(QString::fromStdString(builder.libLinkCmnd()),
               QString::fromStdString(builder.outputDirectory() + "/release"));
  }
}

void MainWindow::archive(Builder &builder) {
  if (ui->actionDebug->isChecked()) {
    runProcess(QString::fromStdString(builder.archiveCmnd()),
               QString::fromStdString(builder.outputDirectory() + "/debug"));
  } else {
    runProcess(QString::fromStdString(builder.archiveCmnd()),
               QString::fromStdString(builder.outputDirectory() + "/release"));
  }
}
