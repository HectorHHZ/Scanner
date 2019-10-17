#ifndef PAINTIMAGEWIDGET_H
#define PAINTIMAGEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

#include <math.h>

class paintimagewidget : public QWidget
{
    Q_OBJECT
public:
    explicit paintimagewidget(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event);

    float twoPointDistance(QPoint point1, QPoint point2);
    bool isValidRect();

    void mouseDoubleClickEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    QPixmap image;
    QPoint top_left_corner;
    QPoint top_right_corner;
    QPoint bottom_left_corner;
    QPoint bottom_right_corner;
    QPoint mouse_position;
    bool mouse_pressed;
    bool valid_rect;

signals:
    void mouseIsMoved(QPixmap subimage);
    void isValid(bool valid_rect);
};

#endif // PAINTIMAGEWIDGET_H
