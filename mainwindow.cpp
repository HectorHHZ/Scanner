#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "edgedetect.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    edgedetect image("C:\\Users\\Leon\\Desktop\\c++data\\15.jpg");
    image.debug();
}

MainWindow::~MainWindow()
{
    delete ui;
}
