#include "Widget.h"
#include "ui_Widget.h"

#include "Patcher.h"
#include "Filer.h"
//#include "DropArea.h"

#include <QFileDialog>
#include <QDateTime>
#include <QPainter>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qRegisterMetaType<ColError>("ColError");

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

    initLogArea();
}

void Widget::openCG45File()
{
    QString fileNameCG45 = QFileDialog::getOpenFileName(this,
                                                        tr("Open *.smg file"),
                                                        "",
                                                        tr("SMG Files (*.smg);;All Files (*)"));

    initPatcher(fileNameCG45);
}

void Widget::appendToLog(ColError err, QString aString)
{
    QString colorString = "%1";
    switch (err) {
        case Error: {
            colorString = tr("<strong><font color=red>[E]: %1</font></strong>");
            break;
        }
        case Warning: {
            colorString = tr("<font color=orange>[W]: %1</font>");
            break;
        }
        case Success: {
            colorString = tr("<font color=green>[S]: %1</font>");
            break;
        }
        case Message:
        default: {
            colorString = tr("<font color=blue>[M]: %1</font>");
            break;
        }
    }

    ui->textBrowser->append(colorString.arg(aString));
}

void Widget::clearLog()
{
    ui->textBrowser->clear();
    initLogArea();
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

void Widget::initLogArea()
{
    appendToLog(Message, tr("Hello! Now %1").arg(QDateTime::currentDateTime().toString("d/M/yy h:m:s")));
}

void Widget::initPatcher(const QString &aFilePath)
{
    Filer::FileError error = filer->setName(aFilePath);

    if (error != Filer::AllOk) {
        return;
    }

    ui->lineEditFilePath->setText(filer->getDirName() + filer->getFileName());
    patcher->setFileName(filer->getFileName());
    patcher->setDirName(filer->getDirName());
    patcher->setPatchedFileName(filer->getPatchedFileName());
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, QPixmap("://gfx/header.png"));
}

void Widget::startPatchThread()
{
    patcher->start(QThread::IdlePriority);
}

void Widget::setProgress(int aValue)
{
    ui->progressBar->setValue(aValue);
}

Widget::~Widget()
{
    delete patcher;
    delete filer;
    delete ui;
}


