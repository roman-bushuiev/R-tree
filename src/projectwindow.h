#ifndef PROJECTWINDOW_H
#define PROJECTWINDOW_H

#include "crtree.h"
#include <QDialog>
#include <string>


namespace Ui {
class ProjectWindow;
}

class ProjectWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectWindow(QWidget *parent = nullptr, string name = "");
    ~ProjectWindow();

    void create_rtree();

private slots:
    void on_search_element_clicked();

    void on_insert_element_clicked();

    void on_delete_element_clicked();

    void on_search_knn_clicked();

private:
    Ui::ProjectWindow *ui;
    std::string project_name;
    CRTree rtree;
};

#endif // PROJECTWINDOW_H
