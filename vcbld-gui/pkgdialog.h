#ifndef PKGDIALOG_H
#define PKGDIALOG_H

#include <QMainWindow>

namespace Ui {
class PkgDialog;
}

class PkgDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit PkgDialog(QWidget *parent = nullptr);
    ~PkgDialog();

private:
    Ui::PkgDialog *ui;
};

#endif // PKGDIALOG_H
