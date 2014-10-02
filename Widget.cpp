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

    patcher = new Patcher(this);

    setFixedSize(556, 439);

    //connect(ui->frameDropFile, SLOT());
    connect(ui->pushButtonOpenCG45, SIGNAL(clicked()), this, SLOT(openCG45File()));
    connect(ui->pushButtonStartPatch, SIGNAL(clicked()), patcher, SLOT(startPatch()));
    connect(patcher, SIGNAL(toLogArea(QString)), this, SLOT(appendToLog(QString)));
    connect(patcher, SIGNAL(clearLogArea()), this, SLOT(clearLog()));

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

void Widget::retranslateUi()
{
    textFileDialogCaption = tr("Open *.smg file");
    textFileDialogFilter = tr("SMG Files (*.smg);;All Files (*)");
}

Widget::~Widget()
{
    delete ui;
}


