#ifndef REMOVE_H
#define REMOVE_H

#include <QListWidgetItem>
#include <QMainWindow>

#include "pkg.h"

namespace Ui {
class Remove;
}

class Remove : public QMainWindow {
  Q_OBJECT

public:
  explicit Remove(QWidget *parent = nullptr);
  ~Remove();
  QString output() const;

private slots:
  void on_cancelButton_clicked();

  void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

signals:
  void outputChanged(const QString &);

private:
  vcbld::PkgClass pkgClass;
  Ui::Remove *ui;
  QString _output;
};

#endif // REMOVE_H
