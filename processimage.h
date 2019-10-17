#ifndef PROCESSIMAGE_H
#define PROCESSIMAGE_H

#include <QWidget>

namespace Ui {
class processimage;
}

class processimage : public QWidget
{
    Q_OBJECT

public:
    explicit processimage(QWidget *parent = 0);
    ~processimage();

    QString getFilepath();
    void setFilepath(QString path);
    QPoint getTopLeftCorner();
    QPoint getTopRightCorner();
    QPoint getBottomLeftCorner();
    QPoint getBottomRightCorner();
    void imageChanged();
protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void back_display(int index);
    void next_display(int index);

private slots:
    void OndoneClicked();
    void OnpaintimageMove(QPixmap subimage);
    void isValidSubimage(bool valid_rect);
    void OnbackClicked();
    void OnnextClicked();

private:
    Ui::processimage *ui;
    QString filepath;
};

#endif // PROCESSIMAGE_H
