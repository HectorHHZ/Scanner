#include "paintimagewidget.h"

#include <QDebug>

paintimagewidget::paintimagewidget(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(450, 600);
    this->setCursor(Qt::OpenHandCursor);

    top_left_corner = QPoint(30, 30);
    top_right_corner = QPoint(this->width() - 30, 30);
    bottom_left_corner = QPoint(30, this->height() - 30);
    bottom_right_corner = QPoint(this->width() - 30, this->height() - 30);

    mouse_pressed = false;
    setMouseTracking(true);
}

void paintimagewidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    int width = this->width();
    int height = this->height();

    if (!image.isNull())
    {
        QRect target(0, 0, width, height);
        QRect source(0, 0, image.width(), image.height());
        painter.drawPixmap(target, image, source);
    }

    painter.setPen(QColor(255, 0, 0));

    /* Paint the top left corner */
    painter.drawLine(QPoint(top_left_corner.x() - 10, top_left_corner.y()),
                     QPoint(top_left_corner.x() + 10, top_left_corner.y()));
    painter.drawLine(QPoint(top_left_corner.x(), top_left_corner.y() + 10),
                     QPoint(top_left_corner.x(), top_left_corner.y() - 10));

    /* Paint the top right corner */
    painter.drawLine(QPoint(top_right_corner.x() - 10, top_right_corner.y()),
                     QPoint(top_right_corner.x() + 10, top_right_corner.y()));
    painter.drawLine(QPoint(top_right_corner.x(), top_right_corner.y() + 10),
                     QPoint(top_right_corner.x(), top_right_corner.y() - 10));

    /* Paint the bottom left corner */
    painter.drawLine(QPoint(bottom_left_corner.x() - 10, bottom_left_corner.y()),
                     QPoint(bottom_left_corner.x() + 10, bottom_left_corner.y()));
    painter.drawLine(QPoint(bottom_left_corner.x(), bottom_left_corner.y() + 10),
                     QPoint(bottom_left_corner.x(), bottom_left_corner.y() - 10));

    /* Paint the bottom right corner */
    painter.drawLine(QPoint(bottom_right_corner.x() - 10, bottom_right_corner.y()),
                     QPoint(bottom_right_corner.x() + 10, bottom_right_corner.y()));
    painter.drawLine(QPoint(bottom_right_corner.x(), bottom_right_corner.y() + 10),
                     QPoint(bottom_right_corner.x(), bottom_right_corner.y() - 10));

    /* Paint the rectangle based on the four corners */
    if (isValidRect())
    {
        painter.setPen(QColor(0, 255, 0));
    }
    else
    {
        painter.setPen(QColor(255, 0, 0));
    }
    painter.drawLine(top_left_corner, top_right_corner);
    painter.drawLine(top_left_corner, bottom_left_corner);
    painter.drawLine(top_right_corner, bottom_right_corner);
    painter.drawLine(bottom_left_corner, bottom_right_corner);

    /* Paint the current mouse position */
    if (mouse_pressed)
    {
    painter.setPen(QColor(0xFF, 0, 0));
    painter.drawLine(QPoint(mouse_position.x() - 10, mouse_position.y()),
                     QPoint(mouse_position.x() + 10, mouse_position.y()));
    painter.drawLine(QPoint(mouse_position.x(), mouse_position.y() + 10),
                     QPoint(mouse_position.x(), mouse_position.y() - 10));
    }
}

void paintimagewidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
}

void paintimagewidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mouse_pressed = true;
    this->setCursor(Qt::ClosedHandCursor);
}

void paintimagewidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mouse_pressed = false;
    this->setCursor(Qt::OpenHandCursor);
    update();
}

void paintimagewidget::mouseMoveEvent(QMouseEvent *event)
{
    mouse_position = event->pos();

    /* Select a region to magnify the image. */
    double widrat = (float)image.width() / this->width();
    double heirat = (float)image.height() / this->height();
    if (mouse_position.x() < 25)
    {
        if (mouse_position.y() < 25)
        {
            QRect magnifiedArea(QPoint(0, 0),
                                QPoint(50 * widrat, 50 * heirat));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
        else if (mouse_position.y() > (this->height() - 25))
        {
            QRect magnifiedArea(QPoint(0, (this->height() - 50) * heirat),
                                QPoint(50 * widrat, image.height()));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
        else
        {
            QRect magnifiedArea(QPoint(0, (mouse_position.y() - 25) * heirat),
                                QPoint(50 * widrat, (mouse_position.y() + 25) * heirat));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
    }
    else if (mouse_position.x() > (this->width() - 25))
    {
        if (mouse_position.y() < 25)
        {
            QRect magnifiedArea(QPoint((this->width() - 50) * widrat, 0),
                                QPoint((this->width()) * widrat, 50 * heirat));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
        else if (mouse_position.y() > (this->height() - 25))
        {
            QRect magnifiedArea(QPoint((this->width() - 50) * widrat, (this->height() - 50) * heirat),
                                QPoint(image.width(), image.height()));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
        else
        {
            QRect magnifiedArea(QPoint((this->width() - 50) * widrat, (mouse_position.y() - 25) * heirat),
                                QPoint(image.width(), (mouse_position.y() + 25) * heirat));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
    }
    else
    {
        if (mouse_position.y() < 25)
        {
            QRect magnifiedArea(QPoint((mouse_position.x() - 25) * widrat, 0),
                                QPoint((mouse_position.x() + 25) * widrat, 50 * heirat));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
        else if (mouse_position.y() > 575)
        {
            QRect magnifiedArea(QPoint((mouse_position.x() - 25) * widrat, (this->height() - 50) * heirat),
                                QPoint((mouse_position.x() + 25) * widrat, image.height()));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
        else
        {
            QRect magnifiedArea(QPoint((mouse_position.x() - 25) * widrat, (mouse_position.y() - 25) * heirat),
                                QPoint((mouse_position.x() + 25) * widrat, (mouse_position.y() + 25) * heirat));
            QPixmap subimage = image.copy(magnifiedArea);
            emit mouseIsMoved(subimage);
        }
    }

    if (mouse_pressed)
    {
        /* Calculate the distance between mouse and four corner, select the closest
         * one, if the distance is smaller than 15, update the corner position
         */
        float distance_top_left = twoPointDistance(mouse_position, top_left_corner);
        float distance_top_right = twoPointDistance(mouse_position, top_right_corner);
        float distance_bottom_left = twoPointDistance(mouse_position, bottom_left_corner);
        float distance_bottom_right = twoPointDistance(mouse_position, bottom_right_corner);
        if (distance_top_left <= distance_top_right && distance_top_left <= distance_bottom_left
                && distance_top_left <= distance_bottom_right && distance_top_left <= 40)
        {
            top_left_corner.rx() = mouse_position.x();
            top_left_corner.ry() = mouse_position.y();
        }
        else if (distance_top_right <= distance_top_left && distance_top_right <= distance_bottom_left
                 && distance_top_right <= distance_bottom_right && distance_top_right <= 40)
        {
            top_right_corner.rx() = mouse_position.x();
            top_right_corner.ry() = mouse_position.y();
        }
        else if (distance_bottom_left <= distance_bottom_right && distance_bottom_left <= distance_top_left
                 && distance_bottom_left <= distance_top_right && distance_bottom_left <= 40) {
            bottom_left_corner.rx() = mouse_position.x();
            bottom_left_corner.ry() = mouse_position.y();
        }
        else if (distance_bottom_right <= distance_bottom_left && distance_bottom_right <= distance_top_left
                 && distance_bottom_right <= distance_top_right && distance_bottom_right <= 40)
        {
            bottom_right_corner.rx() = mouse_position.x();
            bottom_right_corner.ry() = mouse_position.y();
        }

        update();
    }
}

float paintimagewidget::twoPointDistance(QPoint point1, QPoint point2)
{
    return sqrt(pow((point1.x() - point2.x()), 2) + pow((point1.y() - point2.y()), 2));
}

bool paintimagewidget::isValidRect()
{
    /* For top left, the y coordinate should be above the line of top_right and bottom_left */
    double a = 0.00001; // Avoid 0 denomenator.
    if (top_left_corner.y() >
            ((top_right_corner.y() - bottom_left_corner.y()) / (top_right_corner.x() - bottom_left_corner.x() + a)) *
            (top_left_corner.x() - bottom_left_corner.x()) + bottom_left_corner.y())
    {
        valid_rect = false;
        emit isValid(valid_rect);
        return false;
    }
    else if (top_right_corner.y() >
               ((top_left_corner.y() - bottom_right_corner.y()) / (top_left_corner.x() - bottom_right_corner.x() + a)) *
               (top_right_corner.x() - bottom_right_corner.x()) + bottom_right_corner.y())
    {
        valid_rect = false;
        emit isValid(valid_rect);
        return false;
    }
    else if (bottom_left_corner.y() <
               ((top_left_corner.y() - bottom_right_corner.y()) / (top_left_corner.x() - bottom_right_corner.x() + a)) *
                (bottom_left_corner.x() - bottom_right_corner.x()) + bottom_right_corner.y())
    {
        valid_rect = false;
        emit isValid(valid_rect);
        return false;
    }
    else if (bottom_right_corner.y() <
               ((top_right_corner.y() - bottom_left_corner.y()) / (top_right_corner.x() - bottom_left_corner.x() + a)) *
               (bottom_right_corner.x() - bottom_left_corner.x()) + bottom_left_corner.y())
    {
        valid_rect = false;
        emit isValid(valid_rect);
        return false;
    }
    else
    {
        valid_rect = true;
        emit isValid(valid_rect);
        return true;
    }
    return false;
}
