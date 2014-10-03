#include "Patcher.h"

#include <QDir>
#include <QDateTime>

#define COUNT_OF_PATCHES 8
#define DELAY 50

Patcher::Patcher() :
    foundPatches(0), appliedPatches(0), countOfPatches(0),
    progressValue(0)
{
    fileNameCG45 = dirNameCG45 = patchedFileNameCG45 = "";

    moveToThread(this);
}

void Patcher::run()
{
    foundPatches = appliedPatches = countOfPatches = 0;

    emit clearLogArea();
    emit toProgressBar(0);
    emit toLogArea(Message, tr("Patching started... %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss")));

    if(createPatchFile()) {
        emit toLogArea(Success, tr("Done: %1/%2 applied patches").arg(appliedPatches).arg(countOfPatches));
        emit toLogArea(Message, tr("Patching ended. %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
    }

    emit done();
}

bool Patcher::createPatchFile()
{
    QFile fileCG45(dirNameCG45 + QDir::separator() + fileNameCG45);
    if (fileCG45.open(QIODevice::ReadOnly)) {
        fileCG45.seek(0);
        QByteArray header = fileCG45.read(16);
        EPhoneModels phone = determinePhoneModel(header);
        fileCG45.seek(0);
        switch (phone) {
            case ZN5: {
                emit toLogArea(Message, tr("CG45 from <strong>Motorola ZN5</strong> (64 MB RAM)"));
                patchCG45to(fileCG45, ZN5);
                break;
            }
            case ZN5Tmobile: {
                emit toLogArea(Message, tr("CG45 from <strong>Motorola ZN5 T-Mobile</strong> (128 MB RAM)"));
                patchCG45to(fileCG45, ZN5Tmobile);
                break;
            }
            case UnknownModel:
            default: {
                emit toLogArea(Error, tr("File isn't recognize! This is CG45.smg?"));
                fileCG45.close();
                return false;
            }
        }
    } else {
        emit toLogArea(Error, tr("Can't open file: %1").arg(fileCG45.errorString()));
        return false;
    }

    fileCG45.close();

    return true;
}

Patcher::EPhoneModels Patcher::determinePhoneModel(const QByteArray &aHeader)
{
    int zn5 = 0;
    int zn5t = 0;
    for (int i = 1; i < 10; i+=8) { // The 1 and 9 bytes in header
        switch ((unsigned char)aHeader.at(i)) { // To unsigned
            case 0x93: { // ZN5
                ++zn5;
                break;
            }
            case 0x97: { // ZN5 T-mobile
                ++zn5t;
                break;
            }
            default: {
                return UnknownModel;
            }
        }
    }

    if (zn5 == 2) {
        return ZN5;
    }

    if (zn5t == 2) {
        return ZN5Tmobile;
    }

    return UnknownModel;
}

bool Patcher::patchCG45to(QFile &aFile, const EPhoneModels aPhone)
{
    QByteArray ramByteArray(aFile.readAll());
    QBuffer buffer(&ramByteArray);
    if (!buffer.open(QIODevice::ReadWrite)) {
        emit toLogArea(Error, tr("QBuffer error: %1").arg(buffer.errorString()));
        return false;
    }

    if (aPhone == UnknownModel) {
        emit toLogArea(Error, tr("Bad CG45 file!"));
        return false;
    }

    // Patch #1
    applyPatch(buffer, 1, 0x0000E8F4, "80");

    // Patch #2
    applyPatch(buffer, 1, 0x0000E901, "20");

    // Patch #3
    applyPatch(buffer, 4, 0x0000ED16, "81C0719F");

    switch (aPhone) {
        case ZN5: {
            // Patch #4
            applyPatch(buffer, 4, 0x00038C10, "8FC0719F");

            // Patch #5
            applyPatch(buffer, 2, 0x0006FEC4, "C090");

            // Patch #6
            applyPatch(buffer, 8, 0x000E90C8, "C090C090C090C090");

            // Patch #7
            applyPatch(buffer, 6, 0x0008E154, "C090C090C090");
            break;
        }
        case ZN5Tmobile: {
            // Patch #4
            applyPatch(buffer, 4, 0x00036870, "8FC0719F");

            // Patch #5
            applyPatch(buffer, 2, 0x0007175C, "C090");

            // Patch #6
            applyPatch(buffer, 8, 0x00096020, "C090C090C090C090");

            // Patch #7
            applyPatch(buffer, 6, 0x00096960, "C090C090C090");
            break;
        }
        default:
        case UnknownModel: {
            return false;
        }
    }

    // Patch #8
    fillBytes(buffer, 1128, 0x003DF800, "FF");

    buffer.close();

    writePatchedCG45file(dirNameCG45 + QDir::separator() + patchedFileNameCG45, ramByteArray);

    return true;
}

void Patcher::patchFound(int aPatch, const QByteArray &aValue)
{
    ++foundPatches;
    ++countOfPatches;
    emit toLogArea(Warning, QString("#%1: Patch 0x%2 is found. Value (HEX) is %3")
                   .arg(countOfPatches)
                   .arg(QString::number(aPatch, 16).toUpper())
                   .arg(QString(aValue.toHex())));
}

void Patcher::patchFound(int aBegin, int aEnd, const QString &aValue)
{
    ++foundPatches;
    ++countOfPatches;
    emit toLogArea(Warning, QString("#%1: Patch filling 0x%2..0x%3 is found. Value (HEX) is %4")
                   .arg(countOfPatches)
                   .arg(QString::number(aBegin, 16).toUpper())
                   .arg(QString::number(aEnd, 16).toUpper())
                   .arg(aValue.toUpper()));
}

void Patcher::patchApplied(int aPatch, const QByteArray &aValue, const QByteArray &hexString)
{
    ++appliedPatches;
    ++countOfPatches;
    emit toLogArea(Success, QString("#%1: Patch 0x%2 is applied. Value (HEX) is %3 -> %4")
                   .arg(countOfPatches)
                   .arg(QString::number(aPatch, 16).toUpper())
                   .arg(QString(aValue.toHex()))
                   .arg(QString(hexString)));
    if (appliedPatches < COUNT_OF_PATCHES) {
        emit toProgressBar(appliedPatches * (100 / COUNT_OF_PATCHES));
    } else {
        emit toProgressBar(100);
    }
}

void Patcher::patchApplied(int aBegin, int aEnd, const QString &aFromValue, const QString &aToValue)
{
    ++appliedPatches;
    ++countOfPatches;
    emit toLogArea(Success, QString("#%1: Patch 0x%2..0x%3 is filled. Value (HEX) is %4 -> %5")
                   .arg(countOfPatches)
                   .arg(QString::number(aBegin, 16).toUpper())
                   .arg(QString::number(aEnd, 16).toUpper())
                   .arg(aFromValue.toUpper())
                   .arg(aToValue.toUpper()));
    if (appliedPatches < COUNT_OF_PATCHES) {
        emit toProgressBar(appliedPatches * (100 / COUNT_OF_PATCHES));
    } else {
        emit toProgressBar(100);
    }
}

void Patcher::applyPatch(QBuffer &aBuffer, int aLength, int aOffset, const QByteArray &hexString)
{
    aBuffer.seek(aOffset);
    QByteArray valueBA = aBuffer.read(aLength);
    aBuffer.seek(aOffset);
    if (valueBA == QByteArray::fromHex(hexString)) {
        patchFound(aOffset, valueBA);
    } else {
        aBuffer.write(QByteArray::fromHex(hexString));
        patchApplied(aOffset, valueBA, hexString);
    }
    aBuffer.seek(0);

    msleep(DELAY);
}

void Patcher::fillBytes(QBuffer &aBuffer, int aLength, int aOffset, const QByteArray &hexString)
{
    aBuffer.seek(aOffset);
    QByteArray fillValues = aBuffer.read(aLength);

    bool isFill = true;
    for (int i = 0; i < aLength; ++i) {
        if (fillValues.at(i) != QByteArray::fromHex(hexString).at(0)) {
            isFill = false;
            break;
        }
    }

    aBuffer.seek(aOffset);
    if (isFill) {
        patchFound(aOffset, aOffset + aLength,
                   QString::number((unsigned char)fillValues.at(0), 16) + "..." +
                   QString::number((unsigned char)fillValues.at(fillValues.size() - 1), 16));
    } else {
        QByteArray fillArray;
        for (int i = 0; i < aLength; ++i) {
            fillArray.append(QByteArray::fromHex(hexString));
        }
        aBuffer.write(fillArray);
        patchApplied(aOffset, aOffset + aLength,
                     QString::number((unsigned char)fillValues.at(0), 16) + "..." +
                     QString::number((unsigned char)fillValues.at(fillValues.size() - 1), 16),
                     QString::number((unsigned char)fillArray.at(0), 16) + "..." +
                     QString::number((unsigned char)fillArray.at(fillArray.size() - 1), 16));
    }
    aBuffer.seek(0);

    msleep(DELAY);
}

bool Patcher::writePatchedCG45file(const QString &aFullName, const QByteArray &aByteArray)
{
    QFile file(aFullName);
    if (!file.open(QIODevice::WriteOnly)) {
        emit toLogArea(Error, tr("Cannot open file for write: %1").arg(file.errorString()));
        return false;
    } else {
        file.write(aByteArray);
        file.close();
        emit toLogArea(Success, tr("File: <strong>%1</strong> successfully writed!").arg(patchedFileNameCG45));
        return true;
    }
}

void Patcher::setFileName(const QString aFileName)
{
    fileNameCG45 = aFileName;
}

void Patcher::setDirName(const QString aDirName)
{
    dirNameCG45 = aDirName;
}

void Patcher::setPatchedFileName(const QString aFileName)
{
    patchedFileNameCG45 = aFileName;
}

Patcher::~Patcher()
{
    /* Empty Destructor */
}
