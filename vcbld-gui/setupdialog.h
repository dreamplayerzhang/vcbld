#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#if defined(_WIN32) || defined(_WIN64)
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

#include <QDialog>
#include <QListWidgetItem>
#include <string>
#include <vector>

namespace fs = std::experimental::filesystem;

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    SetupDialog(const QString &msg, std::vector<fs::path> &vector, QWidget *parent = nullptr);
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
