#include "Widget.h"
#include "ui_Widget.h"

#include "Patcher.h"
#include "Filer.h"

#include <QDebug>
#include <QFileDialog>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qRegisterMetaType<ColError>("ColError");

    textFileDialogTitle = "";
    textFileDialogFilter = "";

    filer = new Filer(this);
    patcher = new Patcher();

    //setFixedSize(556, 439);

    connect(ui->pushButtonOpenCG45, SIGNAL(clicked()), this, SLOT(openCG45File()));
    connect(ui->pushButtonStartPatch, SIGNAL(clicked()), this, SLOT(startPatchThread()));
    connect(patcher, SIGNAL(toLogArea(ColError, QString)), this, SLOT(appendToLog(ColError, QString)));
    connect(patcher, SIGNAL(clearLogArea()), this, SLOT(clearLog()));
    connect(patcher, SIGNAL(started()), this, SLOT(disableButtons()));
    connect(patcher, SIGNAL(finished()), this, SLOT(enableButtons()));
    connect(patcher, SIGNAL(toProgressBar(int)), this, SLOT(setProgress(int)));
    connect(filer, SIGNAL(toLogArea(ColError, QString)), this, SLOT(appendToLog(ColError, QString)));

    retranslateUi();
}

void Widget::openCG45File()
{
    QString fileNameCG45 = QFileDialog::getOpenFileName(this,
                                                        textFileDialogTitle,
                                                        "",
                                                        textFileDialogFilter);

    Filer::FileError error = filer->setName(fileNameCG45);

    if (error != Filer::AllOk) {
        return;
    }

    qDebug() << error;

    if (fileNameCG45 != "") {
        QFileInfo fileInfo(fileNameCG45);
        QString pathToDir = fileInfo.dir().path();
        QFileInfo dirInfo(pathToDir);

        if (!dirInfo.isReadable()) {
            appendToLog(Error, "Dir isn't Readable");
            return;
        }

        if (!dirInfo.isWritable()) {
            appendToLog(Error, "Dir ist't Writable");
            return;
        }

        qDebug() << pathToDir;

        ui->lineEditFilePath->setText(fileNameCG45);
        patcher->setFileName(fileNameCG45);
        patcher->setDirName(pathToDir);
    }
}

void Widget::appendToLog(ColError err, QString aString)
{
    QString colorString = "%1";
    switch (err) {
        case Error: {
            colorString = "<strong><font color=red>[Err]: %1</font></strong>";
            break;
        }
        case Warning: {
            colorString = "<font color=orange>[W]: %1</font>";
            break;
        }
        case Success: {
            colorString = "<font color=green>[S]: %1</font>";
            break;
        }
        case Message:
        default: {
            colorString = "<font color=blue>[M]: %1</font>";
            break;
        }
    }

    ui->textBrowser->append(colorString.arg(aString));
}

void Widget::clearLog()
{
    ui->textBrowser->clear();
}

void Widget::disableButtons()
{
    disableGUIButtons(true);
}

void Widget::enableButtons()
{
    disableGUIButtons(false);
}

void Widget::disableGUIButtons(bool disable)
{
    ui->pushButtonOpenCG45->setDisabled(disable);
    ui->pushButtonStartPatch->setDisabled(disable);

    ui->progressBar->setDisabled(!disable);

    //TODO: ui->label->acceptDrops();
}

void Widget::startPatchThread()
{
    patcher->start(QThread::IdlePriority);
}

void Widget::setProgress(int aValue)
{
    ui->progressBar->setValue(aValue);
}

void Widget::retranslateUi()
{
    textFileDialogTitle = tr("Open *.smg file");
    textFileDialogFilter = tr("SMG Files (*.smg);;All Files (*)");
}

Widget::~Widget()
{
    delete patcher;
    delete ui;
}


