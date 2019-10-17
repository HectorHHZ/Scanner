#ifndef CHOOSEFILE_H
#define CHOOSEFILE_H

#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QUrl>
#include <QList>
#include <QMimeData>

namespace Ui {
class choosefile;
}

class choosefile : public QWidget
{
    Q_OBJECT

public:
    explicit choosefile(QWidget *parent = 0);
    ~choosefile();

    QString getFilepath();
    void setFilepath(QString path);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void next_display(int index);
    void back_display(int index);

private slots:
    void OnbrowseClicked();
    void OnoneA4Checked();
    void OnmoreThanOneA4Checked();
    void OnnextClicked();
    void OnbackClicked();

private:
    Ui::choosefile *ui;
    QString filepath;
};

#endif // CHOOSEFILE_H
