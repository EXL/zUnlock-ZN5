#ifndef PATCHER_H
#define PATCHER_H

#include <QObject>
#include <QFile>
#include <QBuffer>
#include <QThread>

class Patcher : public QThread
{
    Q_OBJECT

public:
    explicit Patcher();
    ~Patcher();

private:
    QString fileNameCG45;
    enum EPhoneModels {
        ZN5,
        ZN5Tmobile,
        UnknownModel
    };
    int foundPatches;
    int appliedPatches;
    int countOfPatches;
    int progressValue;

private:
    bool createPatchFile(const QString &aFileName);
    Patcher::EPhoneModels determinePhoneModel(const QByteArray &aHeader);
    bool patchCG45fromZN5(QFile &aFile);
    bool patchCG45fromZN5Tmobile(QFile &aFile);
    void patchFound(int aPatch, const QByteArray &aValue);
    void patchFound(int aBegin, int aEnd, const QString &aValue);
    void patchApplied(int aPatch, const QByteArray &aValue, const QByteArray &hexString);
    void patchApplied(int aBegin, int aEnd, const QString &aFromValue, const QString &aToValue);
    void applyPatch(QBuffer &aBuffer, int aLength, int aOffset, const QByteArray &hexString);
    void fillBytes(QBuffer &aBuffer, int aLength, int aOffset, const QByteArray &hexString);
    void run();

public:
    void setFileName(const QString aFileName);

signals:
    void toLogArea(QString);
    void clearLogArea();
    void toProgressBar(int);
};

#endif // PATCHER_H
