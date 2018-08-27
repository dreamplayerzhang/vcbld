#include "pkgdialog.h"
#include "ui_pkgdialog.h"

PkgDialog::PkgDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PkgDialog)
{
    ui->setupUi(this);
}

PkgDialog::~PkgDialog()
{
    delete ui;
}
