#ifndef STARTINTERFACE_H
#define STARTINTERFACE_H

#include <QWidget>

namespace Ui {
class StartInterface;
}

class StartInterface : public QWidget
{
    Q_OBJECT

public:
    explicit StartInterface(QWidget *parent = 0);
    ~StartInterface();

signals:
    void display(int index);

private slots:
    void OnstartClicked();

private:
    Ui::StartInterface *ui;
};

#endif // STARTINTERFACE_H
