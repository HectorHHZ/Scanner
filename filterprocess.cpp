#include "filterprocess.h"
#include "ui_filterprocess.h"

#include <QDebug>

filterprocess::filterprocess(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::filterprocess)
{
    ui->setupUi(this);
    ui->A4->setScaledContents(true);

    connect(ui->back,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnbackClicked()));

    connect(ui->save,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnsaveClicked()));

    connect(ui->filterOne,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnfilterOneClicked()));

    connect(ui->filterTwo,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnfilterTwoClicked()));

    connect(ui->filterThree,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnfilterThreeClicked()));

    connect(ui->filterFour,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnfilterFourClicked()));

    connect(ui->contrastSlider,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(OncontrastChanged(int)));

    connect(ui->brightnessSlider,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(OnbrightnessChanged(int)));

}

filterprocess::~filterprocess()
{
    delete ui;
}

void filterprocess::imageChanged()
{
    ui->A4->setPixmap(this->image);
}

void filterprocess::OnbackClicked()
{
    ui->filterOne->setEnabled(true);
    ui->filterTwo->setEnabled(true);
    ui->filterOne->setChecked(false);
    ui->filterTwo->setChecked(false);
    ui->contrastSlider->setValue(10);
    ui->brightnessSlider->setValue(0);
    QIcon iconone(tr(":/new/image/resource/filterone2.png"));
    ui->filterOne->setIcon(iconone);
    emit display(2);
}

void filterprocess::OnsaveClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), ".", tr("PDF Files (*.PDF)"));
    if (!fileName.isEmpty())
    {
        // Creat and open a pdf file
        QFile pdfFile(fileName);
        pdfFile.open(QIODevice::WriteOnly);
        QPdfWriter *pdfWriter = new QPdfWriter(&pdfFile);
        QPainter *pdfPainter = new QPainter(pdfWriter);
        pdfWriter->setPageSize(QPagedPaintDevice::A4);
        pdfWriter->setResolution(300);

        // Write the content of the pdf
        pdfWriter->setCreator("116010147");
        QRect rect = pdfPainter->viewport();
        int widthMlt = rect.width() / this->image.width();
        int heightMlt = (rect.height() - 35) / this->image.height();
        pdfPainter->scale(widthMlt, heightMlt);
        pdfPainter->drawPixmap(50, 50, this->image);

        // Get the personal infomation for the pdf
        QString info1 = ui->name->text();
        QString info2 = ui->studentID->text();
        QString info3 = ui->courseTitle->text();
        QString info  = info1 + "  " + info2 + "  " + info3;

        // Set font for personal information
        int iTop = 0;
        int iContentWidth = 2100;
        QFont font;
        font.setFamily("Times");
        int fontSize = 8;
        font.setPointSize(fontSize);
        pdfPainter->setFont(font);
        pdfPainter->drawText(QRect(0, iTop, iContentWidth, 30), info);

        pdfWriter->newPage();

        // Close the pdf file
        delete pdfPainter;
        delete pdfWriter;
        pdfFile.close();
    }
    else
    {
        return;
    }
}

void filterprocess::OnfilterOneClicked()
{
    if (ui->filterOne->isChecked())
    {
        ui->filterTwo->setEnabled(false);

        QIcon iconone(tr(":/new/image/resource/filterone.png"));
        ui->filterOne->setIcon(iconone);

        cv::Mat result = handwritting_enhance(this->mat);
        this->mat = result.clone();

        cv::cvtColor(result, result, CV_BGR2RGB);
        QImage image((const uchar *) result.data, result.cols,
                     result.rows, result.step, QImage::Format_RGB888);
        image.bits();

        this->setImage(QPixmap(QPixmap::fromImage(image)));
        this->imageChanged();
    }
    else
    {
        ui->filterTwo->setEnabled(true);

        QIcon iconone(tr(":/new/image/resource/filterone2.png"));
        ui->filterOne->setIcon(iconone);

        this->mat = mat_back_up.clone();
        cv::Mat result = mat.clone();

        cv::cvtColor(result, result, CV_BGR2RGB);
        QImage image((const uchar *) result.data, result.cols,
                     result.rows, result.step, QImage::Format_RGB888);
        image.bits();

        ui->contrastSlider->setValue(10);
        ui->brightnessSlider->setValue(0);
        this->setImage(QPixmap(QPixmap::fromImage(image)));
        this->imageChanged();
    }
}

void filterprocess::OnfilterTwoClicked()
{

}

void filterprocess::OnfilterThreeClicked()
{
    cv::Mat result1;
    cv::flip(this->mat, result1, -1);
    this->mat = result1.clone();

    cv::cvtColor(result1, result1, CV_BGR2RGB);
    QImage image((const uchar *) result1.data, result1.cols,
                 result1.rows, result1.step, QImage::Format_RGB888);
    image.bits();

    cv::Mat result2;
    cv::flip(this->mat_back_up, result2, -1);
    this->mat_back_up = result2.clone();

    this->setImage(QPixmap(QPixmap::fromImage(image)));
    this->imageChanged();
}

void filterprocess::OnfilterFourClicked()
{
    this->mat = mat_back_up.clone();
    cv::Mat result = mat.clone();

    cv::cvtColor(result, result, CV_BGR2RGB);
    QImage image((const uchar *) result.data, result.cols,
                 result.rows, result.step, QImage::Format_RGB888);
    image.bits();

    this->setImage(QPixmap(QPixmap::fromImage(image)));
    this->imageChanged();

    ui->contrastSlider->setValue(10);
    ui->brightnessSlider->setValue(0);
    ui->filterOne->setEnabled(true);
    ui->filterTwo->setEnabled(true);
    ui->filterOne->setChecked(false);
    ui->filterTwo->setChecked(false);
    QIcon iconone(tr(":/new/image/resource/filterone2.png"));
    ui->filterOne->setIcon(iconone);
}

void filterprocess::OncontrastChanged(int alpha)
{
    double scaled_alpha = (double)alpha / 10;
    cv::Mat result = adjust_contrast(this->mat, scaled_alpha, brightness);

    cv::cvtColor(result, result, CV_BGR2RGB);
    QImage image((const uchar *) result.data, result.cols,
                 result.rows, result.step, QImage::Format_RGB888);
    image.bits();

    this->contrast = scaled_alpha;
    this->setImage(QPixmap(QPixmap::fromImage(image)));
    this->imageChanged();
}

void filterprocess::OnbrightnessChanged(int beta)
{
    cv::Mat result = adjust_brightness(this->mat, contrast, beta);

    cv::cvtColor(result, result, CV_BGR2RGB);
    QImage image((const uchar *) result.data, result.cols,
                 result.rows, result.step, QImage::Format_RGB888);
    image.bits();

    this->brightness = beta;
    this->setImage(QPixmap(QPixmap::fromImage(image)));
    this->imageChanged();
}

void filterprocess::setImage(QPixmap img)
{
    this->image = img;
}

void filterprocess::setMat(cv::Mat src)
{
    this->mat = src.clone();
    this->mat_back_up = src.clone();
}
