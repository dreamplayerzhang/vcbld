#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <QApplication>
#include <QDebug>

#include "mainwindow.h"

namespace fs = std::experimental::filesystem;

int main(int argc, char *argv[]) {
  fs::path vcbldPath = argv[0];
  vcbldPath = vcbldPath.parent_path();

  QApplication a(argc, argv);
  MainWindow w(argv[0]);

  QFont font = a.font();
  font.setPointSize(12);
  a.setFont(font);

  w.show();

  return a.exec();
}
