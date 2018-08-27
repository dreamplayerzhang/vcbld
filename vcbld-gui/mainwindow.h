#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <QMainWindow>

#include "init.h"
#include "setupdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

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

private:
  void chooser(const QString &, const SetupDialog *);
  void setup(vcbld::Init &);
  void enableMenus();
  Ui::MainWindow *ui;
  QString _dirName;
};

#endif // MAINWINDOW_H
