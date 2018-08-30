#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>
#include <vector>

#include "fs.h"

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog {
  Q_OBJECT

public:
  SetupDialog(const QString &msg, std::vector<fs::path> &vector,
              QWidget *parent = nullptr);
  SetupDialog(QWidget *parent = nullptr) = delete;
  ~SetupDialog();
  int choice();

private slots:

  void on_listWidget_doubleClicked(const QModelIndex &index);

private:
  Ui::SetupDialog *ui;
  QString _msg;
  std::vector<fs::path> _vector;
  int _choice;
};

#endif // SETUPDIALOG_H
