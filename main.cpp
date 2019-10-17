#include "choosefile.h"
#include "processimage.h"
#include "filterprocess.h"
#include "mainwidget.h"
#include "paintimagewidget.h"
#include "edgedetect.h"
#include "filter.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mainwidget w;
    w.show();

    return a.exec();
}
