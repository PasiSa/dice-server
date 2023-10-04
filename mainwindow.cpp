#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->listenButton, SIGNAL(clicked()), this, SLOT(handleButtonPress()));
}

void MainWindow::handleButtonPress()
{
    QString text = ui->portEdit->text();
    qDebug() << "Button pressed: " << text;
}

MainWindow::~MainWindow()
{
    delete ui;
}

