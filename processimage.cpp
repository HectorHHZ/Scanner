#include "processimage.h"
#include "edgedetect.h"
#include "ui_processimage.h"

processimage::processimage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::processimage)
{
    ui->setupUi(this);
    this->filepath = "";
    ui->paintimage->image.load(this->filepath);
    ui->back->installEventFilter(this);
    ui->next->installEventFilter(this);

    connect(ui->done,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OndoneClicked()));

    connect(ui->paintimage,
            SIGNAL(mouseIsMoved(QPixmap)),
            this,
            SLOT(OnpaintimageMove(QPixmap)));

    connect(ui->paintimage,
            SIGNAL(isValid(bool)),
            this,
            SLOT(isValidSubimage(bool)));

    connect(ui->back,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnbackClicked()));

    connect(ui->next,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnnextClicked()));

}

processimage::~processimage()
{
    delete ui;
}

void processimage::OndoneClicked()
{
    if (ui->done->isChecked())
    {
        ui->next->setEnabled(true);
    }
    else
    {
        ui->next->setEnabled(false);
    }
}

void processimage::OnpaintimageMove(QPixmap subimage)
{
    ui->magnifyArea->image = subimage;

    /* Recalculate the position of the cross in magnifyArea */
    if (ui->paintimage->mouse_position.x() < 25)
    {
        if (ui->paintimage->mouse_position.y() < 25)
        {
            ui->magnifyArea->cross.rx() = ui->paintimage->mouse_position.x() * 4;
            ui->magnifyArea->cross.ry() = ui->paintimage->mouse_position.y() * 4;
        }
        else if (ui->paintimage->mouse_position.y() > 575)
        {
            ui->magnifyArea->cross.rx() = ui->paintimage->mouse_position.x() * 4;
            ui->magnifyArea->cross.ry() = ui->paintimage->mouse_position.y() * 4 - 2200;
        }
        else
        {
            ui->magnifyArea->cross.rx() = ui->paintimage->mouse_position.x() * 4;
            ui->magnifyArea->cross.ry() = ui->magnifyArea->height() / 2;
        }
    }
    else if (ui->paintimage->mouse_position.x() > 425)
    {
        if (ui->paintimage->mouse_position.y() < 25)
        {
            ui->magnifyArea->cross.rx() = ui->paintimage->mouse_position.x() * 4 - 1600;
            ui->magnifyArea->cross.ry() = ui->paintimage->mouse_position.y() * 4;
        }
        else if (ui->paintimage->mouse_position.y() > 575)
        {
            ui->magnifyArea->cross.rx() = ui->paintimage->mouse_position.x() * 4 - 1600;
            ui->magnifyArea->cross.ry() = ui->paintimage->mouse_position.y() * 4 - 2200;
        }
        else
        {
            ui->magnifyArea->cross.rx() = ui->paintimage->mouse_position.x() * 4 - 1600;
            ui->magnifyArea->cross.ry() = ui->magnifyArea->height() / 2;
        }
    }
    else
    {
        if (ui->paintimage->mouse_position.y() < 25)
        {
            ui->magnifyArea->cross.rx() = ui->magnifyArea->width() / 2;
            ui->magnifyArea->cross.ry() = ui->paintimage->mouse_position.y() * 4;
        }
        else if (ui->paintimage->mouse_position.y() > 575)
        {
            ui->magnifyArea->cross.rx() = ui->magnifyArea->width() / 2;
            ui->magnifyArea->cross.ry() = ui->paintimage->mouse_position.y() * 4 - 2200;
        }
        else
        {
            ui->magnifyArea->cross.rx() = ui->magnifyArea->width() / 2;
            ui->magnifyArea->cross.ry() = ui->magnifyArea->height() / 2;
        }
    }

    ui->magnifyArea->update();
}

void processimage::isValidSubimage(bool valid_rect)
{
    if (valid_rect)
    {
        ui->done->setEnabled(true);
    }
    else
    {
        ui->done->setEnabled(false);
        ui->done->setChecked(false);
        ui->next->setEnabled(false);
    }
}

void processimage::OnbackClicked()
{
    ui->done->setChecked(false);
    ui->next->setEnabled(false);
    emit back_display(1);
}

void processimage::OnnextClicked()
{
    emit next_display(3);
}

void processimage::imageChanged()
{
    /* Update the image in paintimage */
    ui->paintimage->image.load(this->filepath);

    /* Detect the four corners */
    edgedetect src(this->filepath.toStdString());
    src.process();

    float widrat = (float)ui->paintimage->width() / ui->paintimage->image.width();
    float heirat = (float)ui->paintimage->height() / ui->paintimage->image.height();
    ui->paintimage->top_left_corner = QPoint((int)(src.top_left_corner.x * widrat),
                                             (int)(src.top_left_corner.y * heirat));
    ui->paintimage->top_right_corner = QPoint((int)(src.top_right_corner.x * widrat),
                                              (int)(src.top_right_corner.y * heirat));
    ui->paintimage->bottom_left_corner = QPoint((int)(src.bottom_left_corner.x * widrat),
                                                (int)(src.bottom_left_corner.y * heirat));
    ui->paintimage->bottom_right_corner = QPoint((int)(src.bottom_right_corner.x * widrat),
                                                 (int)(src.bottom_right_corner.y * heirat));

    /* If the detect process fails, just show the default corners */
    if (!ui->paintimage->isValidRect())
    {
        ui->paintimage->top_left_corner = QPoint(30, 30);
        ui->paintimage->top_right_corner = QPoint(ui->paintimage->width() - 30, 30);
        ui->paintimage->bottom_left_corner = QPoint(30, ui->paintimage->height() - 30);
        ui->paintimage->bottom_right_corner = QPoint(ui->paintimage->width() - 30, ui->paintimage->height() - 30);
    }

    ui->paintimage->update();

}

QPoint processimage::getTopLeftCorner()
{
    return ui->paintimage->top_left_corner;
}

QPoint processimage::getTopRightCorner()
{
    return ui->paintimage->top_right_corner;
}

QPoint processimage::getBottomLeftCorner()
{
    return ui->paintimage->bottom_left_corner;
}

QPoint processimage::getBottomRightCorner()
{
    return ui->paintimage->bottom_right_corner;
}

QString processimage::getFilepath()
{
    return this->filepath;
}

void processimage::setFilepath(QString path)
{
    this->filepath = path;
}

bool processimage::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->back)
    {
        if (event->type() == QEvent::Enter)
        {
            ui->back->setStyleSheet("color:green");
            return true;
        }
        else if (event->type() == QEvent::Leave)
        {
            ui->back->setStyleSheet("color::black");
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (watched == ui->next)
    {
        if (event->type() == QEvent::Enter)
        {
            if (ui->next->isEnabled())
            {
                ui->next->setStyleSheet("color:green");
            }
            return true;
        }
        else if (event->type() == QEvent::Leave)
        {
            ui->next->setStyleSheet("color::black");
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return processimage::eventFilter(watched, event);
    }
}
