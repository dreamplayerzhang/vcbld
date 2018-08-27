#ifndef REMOVE_H
#define REMOVE_H

#include <QMainWindow>

namespace Ui {
class Remove;
}

class Remove : public QMainWindow
{
    Q_OBJECT

public:
    explicit Remove(QWidget *parent = nullptr);
    ~Remove();

private:
    Ui::Remove *ui;
};

#endif // REMOVE_H
