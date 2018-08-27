#include "add.h"
#include "ui_add.h"

Add::Add(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Add)
{
    ui->setupUi(this);
}

Add::~Add()
{
    delete ui;
}
