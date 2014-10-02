#include "Patcher.h"

#include <QFile>
#include <QDir>
#include <QDebug>

Patcher::Patcher(QObject *parent) :
    QObject(parent),
    foundPatches(0), appliedPatches(0), countOfPatches(0)
{
    fileNameCG45 = "";
}

void Patcher::setFileName(const QString aFileName)
{
    fileNameCG45 = aFileName;
}

void Patcher::startPatch()
{
    foundPatches = appliedPatches = countOfPatches = 0;
    emit clearLogArea();

    if (fileNameCG45 != "") {
        emit toLogArea(tr("Start Patching..."));

        QStringList stringList = fileNameCG45.split(QDir::separator());
        QString patchfileName = "_patched";
        int _size = stringList.size() - 1;
        QString origFileName = stringList.at(_size);

        qDebug() << stringList << stringList.at(_size);

        if (origFileName.contains('.')) {
            // Find first dot
            int i;
            for (i = 0; i <= origFileName.size(); ++i) {
                if (origFileName.at(i) == '.') {
                    break;
                }
            }
            // Insert string
            patchfileName = origFileName.insert(i, patchfileName);
        } else {
            patchfileName = origFileName + patchfileName;
        }

        qDebug() << patchfileName;

        createPatchFile(patchfileName);

    } else {
        emit toLogArea(tr("Error: File isn't here"));
    }
}

bool Patcher::createPatchFile(const QString &aFileName)
{
    QFile fileCG45(fileNameCG45);
    if (fileCG45.open(QIODevice::ReadOnly)) {
        QByteArray header = fileCG45.read(16);
        EPhoneModels phone = determinePhoneModel(header);
        switch (phone) {
            case ZN5: {
                emit toLogArea(tr("CG45 from Motorola ZN5 (64 MB RAM)"));
                patchCG45fromZN5(fileCG45);
                break;
            }
            case ZN5Tmobile: {
                emit toLogArea(tr("CG45 from Motorola ZN5 T-Mobile (128 MB RAM)"));
                patchCG45fromZN5Tmobile(fileCG45);
                break;
            }
            case UnknownModel:
            default: {
                emit toLogArea(tr("File isn't recognize!"));
                break;
            }
        }
    } else {
        // TODO: Return Err
    }

    fileCG45.close();
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

bool Patcher::patchCG45fromZN5(QFile &aFile)
{
    aFile.seek(0);
    QByteArray br(aFile.readAll());
    QBuffer buffer(&br);
    if (!buffer.open(QIODevice::ReadWrite)) {
        qDebug() << buffer.errorString();
    }

    // Patch #1
    applyPatch(buffer, 1, 0x0000E8F4, "80");

    // Patch #2
    applyPatch(buffer, 1, 0x0000E901, "20");

    // Patch #3
    applyPatch(buffer, 4, 0x0000ED16, "81C0719F");

    // Patch #4
    applyPatch(buffer, 4, 0x00038C10, "8FC0719F");

    // Patch #5
    applyPatch(buffer, 2, 0x0006FEC4, "C090");

    // Patch #6
    applyPatch(buffer, 8, 0x000E90C8, "C090C090C090C090");

    // Patch #7
    applyPatch(buffer, 6, 0x0008E154, "C090C090C090");

    // Patch #8
    fillBytes(buffer, 1128, 0x003DF800, "FF");

    buffer.close();

    QFile file("test_patch");
    file.open(QIODevice::WriteOnly);
    file.write(br);
    file.close();

}

bool Patcher::patchCG45fromZN5Tmobile(QFile &aFile)
{
    aFile.seek(0);
    QByteArray br(aFile.readAll());
    QBuffer buffer(&br);
    if (!buffer.open(QIODevice::ReadWrite)) {
        qDebug() << buffer.errorString();
    }

    // Patch #1
    applyPatch(buffer, 1, 0x0000E8F4, "80");

    // Patch #2
    applyPatch(buffer, 1, 0x0000E901, "20");

    // Patch #3
    applyPatch(buffer, 4, 0x0000ED16, "81C0719F");

    // Patch #4
    applyPatch(buffer, 4, 0x00036870, "8FC0719F");

    // Patch #5
    applyPatch(buffer, 2, 0x0007175C, "C090");

    // Patch #6
    applyPatch(buffer, 8, 0x00096020, "C090C090C090C090");

    // Patch #7
    applyPatch(buffer, 6, 0x00096960, "C090C090C090");

    // Patch #8
    fillBytes(buffer, 1128, 0x003DF800, "FF");

    buffer.close();

    QFile file("test_patch_t_mob");
    file.open(QIODevice::WriteOnly);
    file.write(br);
    file.close();
}

void Patcher::patchFound(int aPatch, const QByteArray &aValue)
{
    ++foundPatches;
    ++countOfPatches;
    emit toLogArea(QString("#%1: Patch 0x%2 is found. Value (HEX) is %3")
                   .arg(countOfPatches)
                   .arg(QString::number(aPatch, 16).toUpper())
                   .arg(QString(aValue.toHex())));
}

void Patcher::patchFound(int aBegin, int aEnd, const QString &aValue)
{
    ++foundPatches;
    ++countOfPatches;
    emit toLogArea(QString("#%1: Patch filling 0x%2..0x%3 is found. Value (HEX) is %4")
                   .arg(countOfPatches)
                   .arg(QString::number(aBegin, 16).toUpper())
                   .arg(QString::number(aEnd, 16).toUpper())
                   .arg(aValue.toUpper()));
}

void Patcher::patchApplied(int aPatch, const QByteArray &aValue, const QByteArray &hexString)
{
    ++appliedPatches;
    ++countOfPatches;
    emit toLogArea(QString("#%1: Patch 0x%2 is applied. Value (HEX) is %3 -> %4")
                   .arg(countOfPatches)
                   .arg(QString::number(aPatch, 16).toUpper())
                   .arg(QString(aValue.toHex()))
                   .arg(QString(hexString)));
}

void Patcher::patchApplied(int aBegin, int aEnd, const QString &aFromValue, const QString &aToValue)
{
    ++foundPatches;
    ++countOfPatches;
    emit toLogArea(QString("#%1: Patch 0x%2..0x%3 is filled. Value (HEX) is %4 -> %5")
                   .arg(countOfPatches)
                   .arg(QString::number(aBegin, 16).toUpper())
                   .arg(QString::number(aEnd, 16).toUpper())
                   .arg(aFromValue.toUpper())
                   .arg(aToValue.toUpper()));
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
}

Patcher::~Patcher()
{
    /* Empty Destructor */
}
