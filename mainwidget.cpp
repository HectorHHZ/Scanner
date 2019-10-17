#include "mainwidget.h"

#include <QDebug>

mainwidget::mainwidget(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(740, 740);
    page_zero = new StartInterface;
    page_one = new choosefile;
    page_two = new processimage;
    page_three = new filterprocess;

    stacklayout = new QStackedLayout;
    mainlayout = new QVBoxLayout;

    stacklayout->addWidget(page_zero);
    stacklayout->addWidget(page_one);
    stacklayout->addWidget(page_two);
    stacklayout->addWidget(page_three);

    mainlayout->setMargin(0);
    mainlayout->addLayout(stacklayout);
    setLayout(mainlayout);

    connect(page_zero,
            SIGNAL(display(int)),
            this,
            SLOT(page_zero_start_clicked(int)));

    connect(page_one,
            SIGNAL(back_display(int)),
            this,
            SLOT(page_one_back_clicked(int)));

    connect(page_one,
            SIGNAL(next_display(int)),
            this,
            SLOT(page_one_next_clicked(int)));

    connect(page_two,
            SIGNAL(back_display(int)),
            this,
            SLOT(page_two_back_clicked(int)));

    connect(page_two,
            SIGNAL(next_display(int)),
            this,
            SLOT(page_two_next_clicked(int)));

    connect(page_three,
            SIGNAL(display(int)),
            this,
            SLOT(page_three_back_clicked(int)));

}

void mainwidget::page_zero_start_clicked(int index)
{
    this->stacklayout->setCurrentIndex(index);
}

void mainwidget::page_one_back_clicked(int index)
{
    this->stacklayout->setCurrentIndex(index);
}

void mainwidget::page_one_next_clicked(int index)
{
    this->page_two->setFilepath(this->page_one->getFilepath());
    this->page_two->imageChanged();
    this->stacklayout->setCurrentIndex(index);
}

void mainwidget::page_two_back_clicked(int index)
{
    this->stacklayout->setCurrentIndex(index);
}

void mainwidget::page_two_next_clicked(int index)
{
    this->page_three->setImage(this->warp());
    this->page_three->imageChanged();
    this->stacklayout->setCurrentIndex(index);
}

void mainwidget::page_three_back_clicked(int index)
{
    this->stacklayout->setCurrentIndex(index);
}

QPixmap mainwidget::warp()
{
    cv::Mat src = cv::imread(this->page_one->getFilepath().toStdString());
    std::vector<cv::Point2f> corners;
    std::vector<cv::Point2f> new_corners;

    float widrat = (float)src.cols / 450;
    float heirat = (float)src.rows / 600;
    cv::Point2f tlc((float)(this->page_two->getTopLeftCorner().x() * widrat),
                    (float)(this->page_two->getTopLeftCorner().y() * heirat));
    cv::Point2f trc((float)(this->page_two->getTopRightCorner().x() * widrat),
                    (float)(this->page_two->getTopRightCorner().y() * heirat));
    cv::Point2f blc((float)(this->page_two->getBottomLeftCorner().x() * widrat),
                    (float)(this->page_two->getBottomLeftCorner().y() * heirat));
    cv::Point2f brc((float)(this->page_two->getBottomRightCorner().x() * widrat),
                    (float)(this->page_two->getBottomRightCorner().y() * heirat));

    // Deal with the case when the paper is put horizantolly.
    if ( (pow((tlc.x - trc.x), 2) + pow((tlc.y - trc.y), 2)) > (pow((tlc.x - blc.x), 2) + pow((tlc.y - blc.y), 2)) )
    {
        cv::Point2f temp = tlc;
        tlc = blc;
        blc = brc;
        brc = trc;
        trc = temp;
    }

    corners.push_back(tlc);
    corners.push_back(trc);
    corners.push_back(blc);
    corners.push_back(brc);
    new_corners.push_back(cv::Point2f(0, 0));
    new_corners.push_back(cv::Point2f(1260, 0));
    new_corners.push_back(cv::Point2f(0, 1780));
    new_corners.push_back(cv::Point2f(1260, 1780));

    cv::Mat new_image = cv::Mat::zeros(1780, 1260, CV_8UC3);
    cv::Mat transmtx = cv::getPerspectiveTransform(corners, new_corners);
    cv::warpPerspective(src, new_image, transmtx, new_image.size());
    this->page_three->setMat(new_image);

    cv::cvtColor(new_image, new_image, CV_BGR2RGB);
    QImage image((const uchar *) new_image.data, new_image.cols, new_image.rows, new_image.step, QImage::Format_RGB888);
    image.bits();

    return QPixmap(QPixmap::fromImage(image));
}
