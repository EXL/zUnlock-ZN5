#include "DropArea.h"

#include <QPainter>
#include <QUrl>
#include <QMimeData>

DropArea::DropArea(QWidget *parent) :
    QLabel(parent)
{
    setMinimumSize(200, 200);
    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setAutoFillBackground(true);
    clear();
}

void DropArea::clear()
{
    setText(labelText);
    setBackgroundRole(QPalette::Dark);
}

void DropArea::dragEnterEvent(QDragEnterEvent *event)
{
    setText(tr("Drop CG45 smg\nfile here"));
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
}

void DropArea::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void DropArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
    event->accept();
}

void DropArea::dropEvent(QDropEvent *event)
{
    QList<QUrl> urlList = event->mimeData()->urls();

    if (urlList.size() > 1) {
        setText(tr("Please drop\nonly one\nelement"));
    } else {
        emit toPatcher(urlList.at(0).toLocalFile());
    }

    setBackgroundRole(QPalette::Dark);

    event->acceptProposedAction();
}

void DropArea::setLabelText(const QString &aText)
{
    labelText = aText;
}

DropArea::~DropArea()
{
    /* Empty Destructor */
}
