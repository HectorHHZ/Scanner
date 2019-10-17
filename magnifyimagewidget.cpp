#include "magnifyimagewidget.h"

#include <QDebug>

magnifyimagewidget::magnifyimagewidget(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(200, 200);
    cross.rx() = this->width() / 2;
    cross.ry() = this->height() / 2;
}

void magnifyimagewidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    int width = this->width();
    int height = this->height();

    QRect target(0, 0, width, height);
    QRect source(0, 0, image.width(), image.height());
    painter.drawPixmap(target, image, source);

    painter.setPen(QColor(0xFF, 0, 0));
    painter.drawLine(QPoint(cross.x() - 10, cross.y()),
                     QPoint(cross.x() + 10, cross.y()));
    painter.drawLine(QPoint(cross.x(), cross.y() + 10),
                     QPoint(cross.x(), cross.y() - 10));
}
