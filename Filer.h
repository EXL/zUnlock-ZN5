#ifndef FILER_H
#define FILER_H

#include "ColorLog.h"

#include <QObject>
#include <QString>
#include <QFileInfo>

class Filer : public QObject
{
    Q_OBJECT

public:
    explicit Filer(QObject *parent = 0);
    ~Filer();

public:
    enum FileError {
        AllOk,
        FIsEmpty,
        FIsDirIsNotFile,
        FIsNotFile,
        FIsNotReadable,
        FIsNotWritable,
        FIsExists,
        FIsTooBig,
        DIsFileIsNotDir,
        DIsEmpty,
        DIsNotReadable,
        DIsNotWritable
    };

public:
    FileError setName(const QString &aFileName);
    QString getDirName() const;
    QString getFileName() const;
    QString getPatchedFileName() const;

private:
    QString fileName;
    QString patchedFileName;
    QString dirName;

private:
    FileError checkFileInfo(const QFileInfo &aFileInfo);
    void generateName(bool aSmgExt);
    int generateNum(const QString &aName, int aStart, bool aSmgExt) const;

signals:
    void toLogArea(ColError, QString);
};

#endif // FILER_H
