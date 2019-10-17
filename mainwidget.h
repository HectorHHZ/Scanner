#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "choosefile.h"
#include "processimage.h"
#include "filterprocess.h"
#include "startinterface.h"

class mainwidget : public QWidget
{
    Q_OBJECT
public:
    explicit mainwidget(QWidget *parent = nullptr);

private:
    StartInterface *page_zero;
    choosefile *page_one;
    processimage *page_two;
    filterprocess *page_three;
    QStackedLayout *stacklayout;
    QVBoxLayout *mainlayout;

    QPixmap warp();

signals:

public slots:
    void page_zero_start_clicked(int index);
    void page_one_back_clicked(int index);
    void page_one_next_clicked(int index);
    void page_two_back_clicked(int index);
    void page_two_next_clicked(int index);
    void page_three_back_clicked(int index);
};

#endif // MAINWIDGET_H
