#ifndef FILTERPROCESS_H
#define FILTERPROCESS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QWidget>
#include <QFileDialog>
#include <QPdfWriter>
#include <QIODevice>
#include <QFont>
#include <QPrinter>
#include <QPainter>
#include <filter.h>

namespace Ui {
class filterprocess;
}

class filterprocess : public QWidget
{
    Q_OBJECT

public:
    explicit filterprocess(QWidget *parent = 0);
    ~filterprocess();

    void setImage(QPixmap img);
    void setMat(cv::Mat src);
    void imageChanged();

signals:
    void display(int index);

private slots:
    void OnbackClicked();
    void OnsaveClicked();
    void OnfilterOneClicked();
    void OnfilterTwoClicked();
    void OnfilterThreeClicked();
    void OnfilterFourClicked();
    void OncontrastChanged(int alpha);
    void OnbrightnessChanged(int beta);

private:
    Ui::filterprocess *ui;

    QPixmap image;
    cv::Mat mat;
    cv::Mat mat_back_up;

    double contrast = 1;
    int brightness = 0;
};

#endif // FILTERPROCESS_H
