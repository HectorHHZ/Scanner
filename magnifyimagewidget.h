#ifndef MAGNIFYIMAGEWIDGET_H
#define MAGNIFYIMAGEWIDGET_H

#include <QWidget>
#include <QPainter>

class magnifyimagewidget : public QWidget
{
    Q_OBJECT
public:
    explicit magnifyimagewidget(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event);

    QPixmap image;
    QPoint cross;

signals:

public slots:
};

#endif // MAGNIFYIMAGEWIDGET_H
