#ifndef WIDGET_H
#define WIDGET_H

#include "ColorLog.h"

#include <QWidget>
#include <QPaintEvent>

class Patcher;
class Filer;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void openCG45File();
    void appendToLog(ColError err, QString aString);
    void clearLog();
    void disableButtons();
    void enableButtons();
    void startPatchThread();
    void setProgress(int aValue);
    void initPatcher(const QString &aFilePath, bool drop);
    void showAbout();

private:
    void disableGUIButtons(bool disable);
    void initLogArea();

protected:
    void paintEvent(QPaintEvent *);

private:
    QPixmap zn5pix;
    QPixmap header;
    int zn5pixWidth;
    Patcher *patcher;
    Filer *filer;
    Ui::Widget *ui;
};

#endif // WIDGET_H
