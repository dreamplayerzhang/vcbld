#ifndef ADD_H
#define ADD_H

#include <QMainWindow>

namespace Ui {
class Add;
}

class Add : public QMainWindow
{
    Q_OBJECT

public:
    explicit Add(QWidget *parent = nullptr);
    ~Add();

private:
    Ui::Add *ui;
};

#endif // ADD_H
