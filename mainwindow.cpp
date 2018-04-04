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
    edgedetect image("D:\\QT_CREATER\\QTcode\\figure\\1.bmp");
    image.debug();
}

MainWindow::~MainWindow()
{
    delete ui;
}
