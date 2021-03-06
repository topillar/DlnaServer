#ifndef TST_DLNAVIDEOITEM_H
#define TST_DLNAVIDEOITEM_H

#include <QObject>
#include <QtTest>

#include "dlnavideofile.h"

class tst_dlnavideoitem : public QObject
{
    Q_OBJECT
public:
    explicit tst_dlnavideoitem(QObject *parent = 0);

signals:
    void bytesSent(const qint64 &size, const qint64 &towrite);
    void startTranscoding();

public slots:
    void receivedTranscodedData(const QByteArray &data);
    void transcodingOpened();
    void LogMessage(const QString &message);

private Q_SLOTS:
    void testCase_DlnaVideoItem_MKV_MPEG2_AC3();
    void testCase_DlnaVideoItem_MKV_Looper_MPEG2_AC3();
    void testCase_DlnaVideoItem_AVI_Starwars_MPEG4_AAC();
    void testCase_DlnaVideoItem_AVI_Starwars_MPEG2_AC3();

private:
    TranscodeProcess* transcodeProcess;
    long transcodedSize;
    QElapsedTimer transcodeTimer;
    qint64 timeToOpenTranscoding;
};

#endif // TST_DLNAVIDEOITEM_H
