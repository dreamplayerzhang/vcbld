#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QProcess>

#include "builder.h"
#include "fs.h"
#include "init.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(const fs::path vcbldPath, QWidget *parent = nullptr);
  ~MainWindow();
  void runProcess(const QString &process, const QString &dir);

public slots:
  void on_outputChanged(const QString &);
  void local_outputChanged();

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

  void on_actionRemove_2_triggered();

  void on_actionList_3_triggered();

  void on_actionRestore_triggered();

  void on_actionClear_output_2_triggered();

  void on_procFinished(int);

signals:
  void outputChanged(const QString &);

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
  void exec(std::function<void(void)>);
  void execArgs(std::function<void(const std::string &)> func,
                const std::string &arg);
};

#endif // MAINWINDOW_H
