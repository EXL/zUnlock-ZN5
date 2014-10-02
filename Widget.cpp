#include "Widget.h"
#include "ui_Widget.h"

#include "Patcher.h"

#include <QDebug>
#include <QFileDialog>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    textFileDialogCaption = "";
    textFileDialogFilter = "";

    patcher = new Patcher();

    //setFixedSize(556, 439);

    ui->progressBar->setDisabled(true);

    connect(ui->pushButtonOpenCG45, SIGNAL(clicked()), this, SLOT(openCG45File()));
    connect(ui->pushButtonStartPatch, SIGNAL(clicked()), this, SLOT(startPatchThread()));
    connect(patcher, SIGNAL(toLogArea(QString)), this, SLOT(appendToLog(QString)));
    connect(patcher, SIGNAL(clearLogArea()), this, SLOT(clearLog()));
    connect(patcher, SIGNAL(started()), this, SLOT(disableButtons()));
    connect(patcher, SIGNAL(finished()), this, SLOT(enableButtons()));
    connect(patcher, SIGNAL(toProgressBar(int)), this, SLOT(setProgress(int)));

    retranslateUi();
}

void Widget::openCG45File()
{
    QString fileNameCG45 = QFileDialog::getOpenFileName(this,
                                                        textFileDialogCaption,
                                                        "",
                                                        textFileDialogFilter);

    if (fileNameCG45 != "") {
        ui->lineEditFilePath->setText(fileNameCG45);
        patcher->setFileName(fileNameCG45);
    }
}

void Widget::appendToLog(QString aString)
{
    ui->textBrowser->append(aString);
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
    textFileDialogCaption = tr("Open *.smg file");
    textFileDialogFilter = tr("SMG Files (*.smg);;All Files (*)");
}

Widget::~Widget()
{
    delete patcher;
    delete ui;
}


