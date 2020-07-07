#include "createprojectwindow.h"
#include "ui_createprojectwindow.h"
#include "crtree.h"
#include <string>

CreateProjectWindow::CreateProjectWindow(QWidget *parent, string path) :
    QDialog(parent),
    ui(new Ui::CreateProjectWindow), path(path)
{
    ui->setupUi(this);

    QColor color(QWidget::palette().color(QWidget::backgroundRole()));
    QString style("background-color: rgb(60,179,113);");

    ui->create_button->setStyleSheet( style );
}

CreateProjectWindow::~CreateProjectWindow()
{
    delete ui;
}

void CreateProjectWindow::on_create_button_clicked()
{
    std::string project_name = path + "/" + ui->project_name->text().toUtf8().constData();
    uint32_t dim = std::strtoul( ui->dimension->text().toUtf8().constData(), nullptr, 10 );
    uint32_t min_child_nodes = std::strtoul( ui->child_nodes_min->text().toUtf8().constData(), nullptr, 10 );
    uint32_t max_child_nodes = std::strtoul( ui->child_nodes_max->text().toUtf8().constData(), nullptr, 10 );
    uint32_t cache_size = std::strtoul( ui->cache_size->text().toUtf8().constData(), nullptr, 10 );
    uint32_t erased_max = std::strtoul( ui->erase_trashhold->text().toUtf8().constData(), nullptr, 10 );

    CRTree( project_name, dim, min_child_nodes, max_child_nodes, cache_size, erased_max );

    this->close();
}
