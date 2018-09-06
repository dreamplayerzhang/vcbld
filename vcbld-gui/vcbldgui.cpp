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

  QApplication app(argc, argv);
  MainWindow window(vcbldPath);

#if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
  QFont font = app.font();
  font.setPointSize(font.pointSize() - 1);
  app.setFont(font);
#endif

  window.show();

  return app.exec();
}
