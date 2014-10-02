#include "Filer.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

Filer::Filer(QObject *parent) :
    QObject(parent)
{
    fileName = dirName = patchedFileName = "";
}

Filer::FileError Filer::setName(const QString &aFileName)
{
    fileName.clear();
    dirName.clear();
    patchedFileName.clear();

    // File Name
    if (aFileName.isNull()) {
        toLogArea(Error, tr("File name is empty!"));
        return FIsEmpty;
    }
    fileName = aFileName;
    QFileInfo fileInfo(fileName);
    FileError error = checkFileInfo(fileName);
    if (error != AllOk) {
        return error;
    }
    fileName = fileInfo.fileName();

    // Directory Name
    dirName = fileInfo.absolutePath();
    QFileInfo dirInfo(dirName);
    if (dirName.isNull()) {
        toLogArea(Error, tr("Directory name is empty!"));
        return DIsEmpty;
    }
    if (!dirInfo.isDir()) {
        toLogArea(Error, tr("This is not directory!"));
        return DIsFileIsNotDir;
    }
    if (!dirInfo.isReadable()) {
        toLogArea(Error, tr("Directory is not readable!"));
        return DIsNotReadable;
    }
    if (!dirInfo.isWritable()) {
        toLogArea(Error, tr("Directory is not writable!"));
        return DIsNotWritable;
    }
    dirName += QDir::separator();

    // Patched File Name
    QString date = QString("_") + QDateTime::currentDateTime().toString("dd-MM-yy_HH-mm-ss");
    QString patched = "_patched";
    if (!fileName.contains(patched)) {
        QString origFileName = fileInfo.fileName();
        int _size = origFileName.size() - 1;
        if (origFileName.contains('.')) {
            // Find last dot
            int i;
            for (i = _size; i >= 0; --i) {
                if (origFileName.at(i) == '.') {
                    break;
                }
            }
            // Insert string
            patchedFileName = origFileName.insert(i, date + patched);
        } else {
            patchedFileName = origFileName + date + patched;
        }
    } else {
        patchedFileName = fileName;
        if (patchedFileName.endsWith(".smg")) {
            generateName(patchedFileName.size() - 5);
        } else {
            generateName(patchedFileName.size() - 1);
        }
    }
    QFileInfo patchedFileInfo(dirName + QDir::separator() + patchedFileName);
    if (patchedFileInfo.exists()) {
        toLogArea(Error, tr("File exists!"));
        return FIsExists;
    }

    return AllOk;
}

Filer::FileError Filer::checkFileInfo(const QFileInfo &aFileInfo)
{
    if (aFileInfo.isDir()) {
        toLogArea(Error, tr("This is directory, not file!"));
        return FIsDirIsNotFile;
    }
    if (!aFileInfo.isFile()) {
        toLogArea(Error, tr("This is not file!"));
        return FIsNotFile;
    }
    if (!aFileInfo.isReadable()) {
        toLogArea(Error, tr("File is not readable!"));
        return FIsNotReadable;
    }
    if (!aFileInfo.isWritable()) {
        toLogArea(Error, tr("File is not writable!"));
        return FIsNotWritable;
    }
    return AllOk;
}

void Filer::generateName(int aSize)
{
    QChar ch = patchedFileName.at(aSize);
    if (!(ch.isDigit())) {
        patchedFileName.insert(aSize + 1, QString::number(0));
    } else {
        int n = QString(ch).toInt();
        ++n;
        patchedFileName.replace(aSize, 1, QString::number(n));
    }
}

QString Filer::getDirName() const
{
    return dirName;
}

QString Filer::getFileName() const
{
    return fileName;
}

QString Filer::getPatchedFileName() const
{
    return patchedFileName;
}

Filer::~Filer()
{
    /* Empty Destructor */
}
