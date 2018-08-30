#include "qpch.h"

using namespace vcbld;

int main(int argc, char *argv[]) {
  fs::path vcbldPath = argv[0];
  vcbldPath = vcbldPath.parent_path();

  QApplication a(argc, argv);
  MainWindow w(vcbldPath);

  QFont font = a.font();
  font.setPointSize(font.pointSize() - 1);
  a.setFont(font);

  w.show();

  return a.exec();
}
