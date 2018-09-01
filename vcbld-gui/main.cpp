#include "qpch.h"

using namespace vcbld;

int main(int argc, char *argv[]) {
  fs::path vcbldPath = argv[0];
  vcbldPath = vcbldPath.parent_path();

#if defined(__APPLE__) && defined(__MACH__)
  if (vcbldPath.filename().string() == "MacOS") {
    vcbldPath = vcbldPath.parent_path().parent_path().parent_path();
  }
#endif

#if defined(Q_OS_WIN)
#include <windows.h>
  ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
#endif

  QApplication a(argc, argv);
  MainWindow w(vcbldPath);

  QFont font = a.font();
  font.setPointSize(font.pointSize() - 1);
  a.setFont(font);

  w.show();

  return a.exec();
}
