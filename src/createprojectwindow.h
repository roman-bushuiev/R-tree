#ifndef CREATEPROJECTWINDOW_H
#define CREATEPROJECTWINDOW_H
#include <QDialog>
#include <string>

namespace Ui {
class CreateProjectWindow;
}

class CreateProjectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit CreateProjectWindow(QWidget *parent = nullptr, std::string path = "");
    ~CreateProjectWindow();

private slots:

    void on_create_button_clicked();

private:
    Ui::CreateProjectWindow *ui;
    std::string path;
};

#endif // CREATEPROJECTWINDOW_H
