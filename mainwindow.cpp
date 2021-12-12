#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SameBinary.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{

    try
    {
        SameBinary same(ui->Dir1->text().toStdString(), ui->Dir2->text().toStdString());
        std::string result = same.getAllSameInStr();
        QMessageBox::information(this, "Same file", result.c_str());
    }
    catch(const char * ex)
    {
        QMessageBox::critical(this, "Error", ex);
    }
}

