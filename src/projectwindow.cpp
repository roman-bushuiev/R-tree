#include "projectwindow.h"
#include "ui_projectwindow.h"
#include "crtree.h"
#include "doublevector.h"
#include <QStringListModel>
#include <QStatusBar>
#include <QMessageBox>
#include <iostream>
#include <list>
#include <tuple>
#include <vector>

ProjectWindow::ProjectWindow(QWidget *parent, string name) :
    QDialog(parent),
    ui(new Ui::ProjectWindow), rtree( name )
{
    ui->setupUi(this);

    QColor color(QWidget::palette().color(QWidget::backgroundRole()));
    QString style("background-color: rgb(60,179,113);");

    ui->insert_grid->setStyleSheet( style );
    ui->delete_grid->setStyleSheet( style );
    ui->search_grid->setStyleSheet( style );
    ui->knn_grid->setStyleSheet( style );

    QString zero_vector = QString::fromStdString( str_zero_vector( rtree.getDim() ) );

    ui->starting_point_vector_insert->setPlaceholderText( zero_vector );
    ui->starting_point_vector_knn ->setPlaceholderText( zero_vector );
    ui->starting_point_vector->setPlaceholderText( zero_vector );
    ui->distances_vector->setPlaceholderText( zero_vector );
    ui->distances_vector_insert->setPlaceholderText( zero_vector );
    ui->k->setPlaceholderText( "0" );
    ui->id_delete->setPlaceholderText( "0" );
    ui->id_insert->setPlaceholderText( "0" );
}

ProjectWindow::~ProjectWindow()
{
    delete ui;
}

void ProjectWindow::on_search_element_clicked()
{
    bool exception_caught = false;
    vector<double> start;
    vector<double> dist;

    try
    {
        start = stodv( ui->starting_point_vector->text().toUtf8().constData() );
        dist = stodv( ui->distances_vector->text().toUtf8().constData() );
    }catch( std::invalid_argument e )
    {
        exception_caught = true;
        QMessageBox::critical( this, "Warning", QString::fromStdString( e.what() ) );
        ui->status_bar->setText( QString::fromStdString( e.what() ) );
    }

    std::list<tuple<uint32_t, vector<double>, vector<double>>> res;
    string time;

    try
    {
        clock_t time_a = clock();
        res = rtree.search( start, dist );
        clock_t time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            time = "Unable to calculate time";
        else
            time = to_string( (unsigned)( time_b - time_a ) );
    }catch( std::logic_error e )
    {
        exception_caught = true;
        QMessageBox::critical( this, "Warning", QString::fromStdString( e.what() ) );
        ui->status_bar->setText( QString::fromStdString( e.what() ) );
    }

    if( ! exception_caught )
    {
        QStringListModel * model = new QStringListModel();
        QStringList list;

        for( auto & object : res )
        {
            list << "Id: " + QString::fromStdString( std::to_string( std::get<0>( object ) ) )
                  + "; Starting point vector: " + dvtoqs( std::get<1>( object ) )
                  + "; Distances vector: " + dvtoqs( std::get<2>( object ) );
        }

        model->setStringList(list);
        ui->list_view->setModel(model);
        ui->list_view->update();

        ui->status_bar->setText( QString::fromStdString( "Search time: " + time +
                                                         "ms, I/O operations number: " + to_string( rtree.lastOpIO() ) ) );
    }
}

void ProjectWindow::on_insert_element_clicked()
{
    bool exception_caught = false;
    uint32_t id;
    vector<double> start;
    vector<double> dist;

    try
    {
        id = std::stoi( ui->id_insert->text().toUtf8().constData() );
        start = stodv( ui->starting_point_vector_insert->text().toUtf8().constData() );
        dist = stodv( ui->distances_vector_insert->text().toUtf8().constData() );
    }catch ( std::invalid_argument e )
    {
        exception_caught = true;
        QMessageBox::critical( this, "Warning", QString::fromStdString( e.what() ) );
        ui->status_bar->setText( QString::fromStdString( e.what() ) );
    }

    string time;

    try
    {
        clock_t time_a = clock();
        rtree.insert( id, start, dist );
        clock_t time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            time = "Unable to calculate time";
        else
            time = to_string( (unsigned)( time_b - time_a ) );
    }catch ( std::logic_error e )
    {
        exception_caught = true;
        QMessageBox::critical( this, "Warning", QString::fromStdString( e.what() ) );
        ui->status_bar->setText( QString::fromStdString( e.what() ) );
    }

    if( ! exception_caught )
    {
        ui->status_bar->setText( QString::fromStdString( "The new element was succesfully added. Insert time: " + time +
                                                         "ms, I/O operations number: " + to_string( rtree.lastOpIO() ) ) );
    }
}

void ProjectWindow::on_delete_element_clicked()
{
    uint32_t id = std::stoi( ui->id_delete->text().toUtf8().constData() );
    bool exception_caught = false;
    string time;

    try
    {
        clock_t time_a = clock();
        rtree.erase( id );
        clock_t time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            time = "Unable to calculate time";
        else
            time = to_string( (unsigned)( time_b - time_a ) );
    }catch ( std::logic_error e )
    {
        exception_caught = true;
        QMessageBox::critical( this, "Warning", QString::fromStdString( e.what() ) );
        ui->status_bar->setText( QString::fromStdString( e.what() ) );
    }

    if( ! exception_caught )
    {
        ui->status_bar->setText( QString::fromStdString( "The element was deleted. Delete time: " + time +
                                                         "ms, I/O operations number: " + to_string( rtree.lastOpIO() ) ) );
    }
}

void ProjectWindow::on_search_knn_clicked()
{
    bool exception_caught = false;

    vector<double> start;
    uint32_t k;

    try
    {
        k = std::stoi( ui->k->text().toUtf8().constData() );
        start = stodv( ui->starting_point_vector_knn->text().toUtf8().constData() );
    }catch ( std::invalid_argument e )
    {
        exception_caught = true;
        QMessageBox::critical( this, "Warning", QString::fromStdString( e.what() ) );
        ui->status_bar->setText( QString::fromStdString( e.what() ) );
    }

    std::list<tuple<uint32_t, vector<double>, vector<double>>> res;
    string time;

    try
    {
        clock_t time_a = clock();
        res = rtree.knn( k, start );
        clock_t time_b = clock();

        if ( time_a == ( (clock_t) - 1 ) || time_b == ( (clock_t) - 1 ) )
            time = "Unable to calculate time";
        else
            time = to_string( (unsigned)( time_b - time_a ) );
    } catch ( std::logic_error e )
    {
        exception_caught = true;
        QMessageBox::critical( this, "Warning", QString::fromStdString( e.what() ) );
        ui->status_bar->setText( QString::fromStdString( e.what() ) );
    }

    if( ! exception_caught )
    {
        QStringListModel * model = new QStringListModel();
        QStringList list;

        for( auto & object : res )
        {
            list << "Id: " + QString::fromStdString( std::to_string( std::get<0>( object ) ) )
                  + "; Starting point vector: " + dvtoqs( std::get<1>( object ) )
                  + "; Distances vector: " + dvtoqs( std::get<2>( object ) );
        }

        model->setStringList(list);
        ui->list_view->setModel(model);
        ui->list_view->update();

        ui->status_bar->setText( QString::fromStdString( "Search time: " + time +
                                                         "ms, I/O operations number: " + to_string( rtree.lastOpIO() ) ) );
    }
}
