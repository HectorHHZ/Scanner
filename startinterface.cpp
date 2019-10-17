#include "startinterface.h"
#include "ui_startinterface.h"

StartInterface::StartInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartInterface)
{
    ui->setupUi(this);

    connect(ui->start,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnstartClicked()));

}

StartInterface::~StartInterface()
{
    delete ui;
}

void StartInterface::OnstartClicked()
{
    emit display(1);
}
