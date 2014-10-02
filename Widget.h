#ifndef WIDGET_H
#define WIDGET_H

#include "ColorLog.h"

#include <QWidget>

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

private:
    void disableGUIButtons(bool disable);
    void initLogArea();
    void initPatcher(const QString &aFilePath);

private:
    Patcher *patcher;
    Filer *filer;
    Ui::Widget *ui;
};

#endif // WIDGET_H
