#include "choosefile.h"
#include "ui_choosefile.h"

choosefile::choosefile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::choosefile)
{
    ui->setupUi(this);

    ui->dragArea->installEventFilter(this);
    ui->back->installEventFilter(this);
    ui->next->installEventFilter(this);
    ui->dragArea->setAcceptDrops(true);
    ui->dragArea->setAlignment(Qt::AlignCenter);

    connect(ui->browse,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnbrowseClicked()));

    connect(ui->oneA4,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(OnoneA4Checked()));

    connect(ui->moreThanOneA4,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(OnmoreThanOneA4Checked()));

    connect(ui->next,
           SIGNAL(clicked(bool)),
           this,
           SLOT(OnnextClicked()));

    connect(ui->back,
            SIGNAL(clicked(bool)),
            this,
            SLOT(OnbackClicked()));

}

choosefile::~choosefile()
{
    delete ui;
}

void choosefile::OnbrowseClicked()
{
    QFileDialog *dialog = new QFileDialog(this);
    dialog->setDirectory(".");
    dialog->setFileMode(QFileDialog::AnyFile);
    dialog->setNameFilter(tr("Image (*.jpg *.png *.bmp)"));
    dialog->setViewMode(QFileDialog::Detail);
    if (dialog->exec())
    {
        this->filepath = dialog->selectedFiles()[0];
        ui->chooseFile->setText(filepath);
    }
    delete dialog;
}

void choosefile::OnoneA4Checked()
{
    if ((ui->oneA4->isChecked() && !ui->moreThanOneA4->isChecked())
            || (ui->moreThanOneA4->isChecked() && !ui->oneA4->isChecked()))
    {
        ui->next->setEnabled(true);
    }
    else
    {
        ui->next->setEnabled(false);
    }
}

void choosefile::OnmoreThanOneA4Checked()
{
    if ((ui->moreThanOneA4->isChecked() && !ui->oneA4->isChecked())
            || (ui->oneA4->isChecked() && !ui->moreThanOneA4->isChecked()))
    {
        ui->next->setEnabled(true);
    }
    else
    {
        ui->next->setEnabled(false);
    }
}

void choosefile::OnnextClicked()
{
    this->filepath = ui->chooseFile->text();
    QFileInfo fileInfo = this->filepath;
    if (!fileInfo.isFile() || !fileInfo.exists())
    {
        QMessageBox::about(this, "Error", "Invalid filepath!!");
        return;
    }
    else
    {
        QImage *test = new QImage;
        test->load(this->filepath);
        if (test->isNull())
        {
            QMessageBox::about(this, "Error", "Not a image!");
            delete test;
            return;
        }
        else
        {
            delete test;
            emit next_display(2);
        }
    }
}

void choosefile::OnbackClicked()
{
    emit back_display(0);
}

bool choosefile::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->dragArea)
    {
        if (event->type() == QEvent::DragEnter)
        {
            QDragEnterEvent *dragEnter = dynamic_cast<QDragEnterEvent *>(event);
            dragEnter->acceptProposedAction();
            return true;
        }
        else if (event->type() == QEvent::Drop)
        {
            QDropEvent *drop = dynamic_cast<QDropEvent *>(event);
            QList<QUrl> urls = drop->mimeData()->urls();

            if (urls.isEmpty()) { return true; }
            this->filepath = urls.first().toLocalFile();
            ui->chooseFile->setText(this->filepath);

            QImage image(filepath);
            if (!image.isNull())
            {
                image = image.scaled(ui->dragArea->size(),
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
                ui->dragArea->setPixmap(QPixmap::fromImage(image));
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (watched == ui->back)
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
        return choosefile::eventFilter(watched, event);
    }
}

QString choosefile::getFilepath()
{
    return this->filepath;
}

void choosefile::setFilepath(QString path)
{
    this->filepath = path;
}
