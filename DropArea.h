#ifndef DROPAREA_H
#define DROPAREA_H

#include <QLabel>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>

class QMimeData;

class DropArea : public QLabel
{
    Q_OBJECT

public:
    explicit DropArea(QWidget *parent = 0);
    ~DropArea();

public:
    void setLabelText(const QString &aText);

public slots:
    void clear();

signals:
    void toPatcher(QString);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    QString labelText;
};

#endif // DROPAREA_H
