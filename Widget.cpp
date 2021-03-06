#include "Widget.h"
#include "ui_Widget.h"

#include "Patcher.h"
#include "Filer.h"

#include <QFileDialog>
#include <QDateTime>
#include <QPainter>
#include <QPixmap>
#include <QMessageBox>
#include <QFont>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    qRegisterMetaType<ColError>("ColError");

    filer = new Filer(this);
    patcher = new Patcher();

    header = QPixmap("://gfx/header.png");
    zn5pix = QPixmap("://gfx/zn5.png");
    zn5pixWidth = zn5pix.width();

    setFixedSize(sizeHint());
    setWindowIcon(QIcon("://gfx/ZN5_icon.png"));

    // For monospace font into log output
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    ui->textBrowser->setFont(font);

    ui->labelDropFile->setLabelText(ui->labelDropFile->text());

    connect(ui->pushButtonOpenCG45, SIGNAL(clicked()), this, SLOT(openCG45File()));
    connect(ui->pushButtonAbout, SIGNAL(clicked()), this, SLOT(showAbout()));

    connect(patcher, SIGNAL(toLogArea(ColError, QString)), this, SLOT(appendToLog(ColError, QString)));
    connect(patcher, SIGNAL(clearLogArea()), this, SLOT(clearLog()));
    connect(patcher, SIGNAL(started()), this, SLOT(disableButtons()));
    connect(patcher, SIGNAL(finished()), this, SLOT(enableButtons()));
    connect(patcher, SIGNAL(toProgressBar(int)), this, SLOT(setProgress(int)));
    connect(patcher, SIGNAL(done()), ui->labelDropFile, SLOT(clear()));

    connect(filer, SIGNAL(toLogArea(ColError, QString)), this, SLOT(appendToLog(ColError, QString)));
    connect(ui->labelDropFile, SIGNAL(toPatcher(QString)), this, SLOT(initPatcher(QString)));

    initLogArea();
}

void Widget::openCG45File()
{
    QString fileNameCG45 = QFileDialog::getOpenFileName(this,
                                                        tr("Open *.smg file"),
                                                        "",
                                                        tr("SMG Files (*.smg);;All Files (*)"));

    if (fileNameCG45.isEmpty()) {
        return;
    }

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
    ui->pushButtonAbout->setDisabled(disable);

    ui->progressBar->setDisabled(!disable);
    ui->labelDropFile->setDisabled(disable);
}

void Widget::initLogArea()
{
    appendToLog(Message, tr("Hello! Now %1").arg(QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss")));
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

    startPatchThread();
}

void Widget::showAbout()
{
    QMessageBox::about(this, tr("About zUnlock-ZN5"), tr("<p><strong>Version 0.1</strong></p>"
                                                         "This program helps you to patch the code group (CG45),<br>"
                                                         "for unlock your phone. More details in this "
                                                         "<a href=\"http://forum.motofan.ru/index.php?showtopic=1730577\">forum thread</a><br>"
                                                         "on MotoFan.Ru.<br><br>"
                                                         "Tested on <i>R6637_G_81.03.05R</i> and <i>R6637_G_81.11.2BR_128</i>.<br>"
                                                         "Based on pathes by <a href=\"http://forum.motofan.ru/index.php?showuser=90408\">yakk</a>.<br>"
                                                         "<p><center>EXL, 2014</center></p>"));
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, header);
    painter.drawPixmap(size().width() - zn5pixWidth - 20, 16, zn5pix);
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
