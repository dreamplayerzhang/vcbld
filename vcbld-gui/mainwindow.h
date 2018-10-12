#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <functional>

#include "builder.h"
#include "fs.h"
#include "init.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(const fs::path &vcbldPath, QWidget *parent = nullptr);
  ~MainWindow();
  void runProcess(const QString &process, const QString &dir);

public slots:
  void on_outputChanged(const QString &);
  void local_outputChanged();
  void on_vcpkgCmnd(const std::string &);

private slots:
  void on_actionNew_triggered();
  void on_actionOpen_triggered();
  void on_actionAlways_on_top_triggered(bool checked);
  void on_actionDebug_triggered(bool checked);
  void on_actionRelease_triggered(bool checked);
  void on_actionBuild_triggered();
  void on_actionClean_triggered();
  void on_actionRun_triggered();
  void on_actionRun_Cmake_triggered();
  void on_actionRun_make_triggered();
  void on_actionSetup_triggered();
  void on_actionAbout_triggered();
  void on_actionClose_triggered();
  void on_actionExit_triggered();
  void on_actionHelp_triggered();
  void on_actionIncludePath_triggered();
  void on_actionCMakeLists_triggered();
  void on_actionBuild_run_triggered();
  void on_actionAdd_triggered();
  void on_actionRemove_triggered();
  void on_actionList_triggered();
  void on_actionRestore_triggered();
  void on_actionClear_output_triggered();
  void on_actionToolbarZmIn_triggered();
  void on_actionToolbarZmOut_triggered();
  void on_actionOutputZmIn_triggered();
  void on_actionOutputZmOut_triggered();
  void on_actionInstall_triggered();
  void on_actionUninstall_triggered();
  void on_procFinished(int);
  void on_actionBuild_Commands_triggered();

signals:
  void outputChanged(const QString &);

protected:
  void resizeEvent(QResizeEvent *);

private:
  void setup(vcbld::Init &);
  void enableMenus(bool);
  void clear();
  void compile(vcbld::Builder &);
  void appLink(vcbld::Builder &);
  void libLink(vcbld::Builder &);
  void archive(vcbld::Builder &);
  QLabel *statusLabel;
  QProcess *proc;
  Ui::MainWindow *ui;
  vcbld::Init init;
  QString _dirName;
  fs::path _vcpkgPath;
  fs::path _vcbldPath;
};

#endif // MAINWINDOW_H
