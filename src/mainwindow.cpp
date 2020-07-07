#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "createprojectwindow.h"
#include "projectwindow.h"
#include <QStandardPaths>
#include <QDir>
#include <experimental/filesystem>
#include <string>

#define DB_FOLDER_NAME "rtree_projects"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QColor color(QWidget::palette().color(QWidget::backgroundRole()));
    QString style("background-color: rgb(60,179,113);");

    ui->open_project->setStyleSheet( style );

    projects_dir = QDir( QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) );
    if( ! projects_dir.exists( DB_FOLDER_NAME ) )
        projects_dir.mkdir( DB_FOLDER_NAME );
    projects_dir.cd( QString::fromStdString( DB_FOLDER_NAME ) );

    QStringList files = projects_dir.entryList(QStringList(), QDir::Files);
    for( auto file : files )
        ui->projects_combo_box->addItem( file );
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_create_project_clicked()
{
    CreateProjectWindow create_project_window( nullptr, projects_dir.path().toUtf8().constData() );
    create_project_window.setModal( true );
    create_project_window.exec();

    ui->projects_combo_box->clear();

    QStringList files = projects_dir.entryList(QStringList(), QDir::Files);
    for( auto file : files )
        ui->projects_combo_box->addItem( file );
}

void MainWindow::on_open_project_clicked()
{
    ProjectWindow project_window( nullptr, ( projects_dir.path() + "/" + ui->projects_combo_box->currentText() ).toUtf8().constData() );
    project_window.setModal( false );
    project_window.setWindowTitle( ui->projects_combo_box->currentText() );
    project_window.exec();
}
