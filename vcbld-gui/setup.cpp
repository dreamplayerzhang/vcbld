#include "setup.h"
#include "ui_setup.h"

Setup::Setup(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Setup)
{
    ui->setupUi(this);
}

Setup::~Setup()
{
    delete ui;
}
