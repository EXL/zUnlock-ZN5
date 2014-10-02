#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class Patcher;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public:
    void retranslateUi();

private slots:
    void openCG45File();
    void appendToLog(QString aString);
    void clearLog();
    void disableButtons();
    void enableButtons();
    void startPatchThread();
    void setProgress(int aValue);

private:
    void disableGUIButtons(bool disable);

private:
    QString textFileDialogCaption;
    QString textFileDialogFilter;

private:
    Patcher *patcher;
    Ui::Widget *ui;
};

#endif // WIDGET_H
