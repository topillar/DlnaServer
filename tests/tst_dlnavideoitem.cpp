#include "tst_dlnavideoitem.h"

tst_dlnavideoitem::tst_dlnavideoitem(QObject *parent) :
    QObject(parent),
    transcodeProcess(0),
    transcodeTimer(),
    timeToOpenTranscoding(0)
{
    QFfmpegProcess::setDirPath("/opt/local/bin");
}

void tst_dlnavideoitem::receivedTranscodedData(const QByteArray &data)
{
    transcodedSize += data.size();
    emit bytesSent(transcodedSize, 1);
}

void tst_dlnavideoitem::transcodingOpened()
{
    if (transcodeTimer.isValid())
    {
        timeToOpenTranscoding = transcodeTimer.elapsed();
        qWarning() << "Transcoding opened in" << timeToOpenTranscoding << "ms.";
    }
}

void tst_dlnavideoitem::LogMessage(const QString &message)
{
    qWarning() << message;
}

void tst_dlnavideoitem::testCase_DlnaVideoItem_AVI_Starwars_MPEG4_AAC() {
    Logger log;
    log.setLevel(INF);

    DlnaVideoFile movie(&log, "/Users/doudou/Movies/Films/Fiction/Starwars/Star.Wars.EpisodeIII.La.Revanche.Des.S.avi", "host", 600);
    movie.setTranscodeFormat(MPEG4_AAC);

    QVERIFY(movie.toTranscode()==true);
    QVERIFY(movie.format() == MPEG4_AAC);
    QVERIFY(movie.getSystemName() == "/Users/doudou/Movies/Films/Fiction/Starwars/Star.Wars.EpisodeIII.La.Revanche.Des.S.avi");
    QVERIFY2(movie.bitrate()==4718800, QString("%1").arg(movie.bitrate()).toUtf8().constData());
    QVERIFY2(movie.getLengthInMilliSeconds()==8090173, QString("%1").arg(movie.getLengthInMilliSeconds()).toUtf8().constData());
    QVERIFY2(movie.size()==4771988544, QString("%1").arg(movie.size()).toUtf8().constData());
    QVERIFY2(movie.framerate() == "23.976", movie.framerate().toUtf8());

    QVERIFY2(movie.audioLanguages() == QStringList() << "", movie.audioLanguages().join(',').toUtf8());
    QVERIFY2(movie.subtitleLanguages() == QStringList(), movie.audioLanguages().join(',').toUtf8());

    QVERIFY(movie.getdlnaOrgOpFlags() == "10");
    QVERIFY(movie.getdlnaOrgPN() == "MPEG_PS_PAL");
    QVERIFY(movie.getDlnaContentFeatures() == "DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1");
    QVERIFY2(movie.getProtocolInfo() == "http-get:*:video/mp4:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1", movie.getProtocolInfo().toUtf8());

    QVERIFY(movie.getAlbumArt().isNull() == true);
    QVERIFY(movie.getByteAlbumArt().isNull() == true);

    QHash<QString, double> result = movie.volumeInfo(-1);
    qWarning() << result;
    QVERIFY2(result.keys().size() == 10, QString("%1").arg(result.keys().size()).toUtf8());
    QVERIFY2(result["mean_volume"] == -20.7, QString("%1").arg(result["mean_volume"]).toUtf8());
    QVERIFY2(result["max_volume"] == -0.3, QString("%1").arg(result["max_volume"]).toUtf8());

    Device *device = movie.getStream(0, 0, -1);
    transcodeProcess = qobject_cast<TranscodeProcess*>(device);
    QVERIFY(transcodeProcess != 0);

    transcodedSize = 0;
    connect(this, SIGNAL(startTranscoding()), transcodeProcess, SLOT(startRequestData()));
    connect(this, SIGNAL(bytesSent(qint64,qint64)), transcodeProcess, SLOT(bytesSent(qint64,qint64)));
    connect(transcodeProcess, SIGNAL(sendDataToClientSignal(QByteArray)), this, SLOT(receivedTranscodedData(QByteArray)));
    connect(transcodeProcess, SIGNAL(openedSignal()), this, SLOT(transcodingOpened()));
    connect(transcodeProcess, SIGNAL(LogMessage(QString)), this, SLOT(LogMessage(QString)));
    transcodeTimer.start();
    timeToOpenTranscoding = 0;
    QVERIFY(transcodeProcess->open() == true);
    emit startTranscoding();
    transcodeProcess->waitForFinished(-1);
    QVERIFY(transcodeProcess->bytesAvailable() == 0);
    qint64 duration = transcodeTimer.elapsed();
    qWarning() << "Transcoding opened in" << timeToOpenTranscoding << "ms and finished in" << duration << "ms.";
    QVERIFY2(timeToOpenTranscoding < 700, QString("%1").arg(timeToOpenTranscoding).toUtf8());
    QVERIFY2(duration < 3000000, QString("%1").arg(duration).toUtf8());
    QVERIFY(transcodeProcess->exitCode() == 0);
    qWarning() << "DELTA" << movie.size()-transcodedSize << qAbs(double(movie.size()-transcodedSize))/movie.size();
    QVERIFY(movie.size() > transcodedSize);
    QVERIFY2(transcodedSize == 4760087432, QString("transcoded size = %1").arg(transcodedSize).toUtf8());
    delete transcodeProcess;
    transcodeProcess = 0;
}

void tst_dlnavideoitem::testCase_DlnaVideoItem_AVI_Starwars_MPEG2_AC3() {

    Logger log;
    log.setLevel(INF);

    DlnaVideoFile movie(&log, "/Users/doudou/Movies/Films/Fiction/Starwars/Star.Wars.EpisodeIII.La.Revanche.Des.S.avi", "host", 600);
    movie.setTranscodeFormat(MPEG2_AC3);

    QVERIFY(movie.toTranscode()==true);
    QVERIFY(movie.format() == MPEG2_AC3);
    QVERIFY(movie.getSystemName() == "/Users/doudou/Movies/Films/Fiction/Starwars/Star.Wars.EpisodeIII.La.Revanche.Des.S.avi");
    QVERIFY2(movie.bitrate()==4718800, QString("%1").arg(movie.bitrate()).toUtf8().constData());
    QVERIFY2(movie.getLengthInMilliSeconds()==8090173, QString("%1").arg(movie.getLengthInMilliSeconds()).toUtf8().constData());
    QVERIFY2(movie.size()==4771988544, QString("%1").arg(movie.size()).toUtf8().constData());
    QVERIFY2(movie.framerate() == "23.976", movie.framerate().toUtf8());

    QVERIFY2(movie.audioLanguages() == QStringList() << "", movie.audioLanguages().join(',').toUtf8());
    QVERIFY2(movie.subtitleLanguages() == QStringList(), movie.audioLanguages().join(',').toUtf8());

    QVERIFY(movie.getdlnaOrgOpFlags() == "10");
    QVERIFY(movie.getdlnaOrgPN() == "MPEG_PS_PAL");
    QVERIFY(movie.getDlnaContentFeatures() == "DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1");
    QVERIFY2(movie.getProtocolInfo() == "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1", movie.getProtocolInfo().toUtf8());

    QVERIFY(movie.getAlbumArt().isNull() == true);
    QVERIFY(movie.getByteAlbumArt().isNull() == true);

    QHash<QString, double> result = movie.volumeInfo(-1);
    qWarning() << result;
    QVERIFY2(result.keys().size() == 10, QString("%1").arg(result.keys().size()).toUtf8());
    QVERIFY2(result["mean_volume"] == -20.7, QString("%1").arg(result["mean_volume"]).toUtf8());
    QVERIFY2(result["max_volume"] == -0.3, QString("%1").arg(result["max_volume"]).toUtf8());

    Device *device = movie.getStream(0, 0, -1);
    transcodeProcess = qobject_cast<TranscodeProcess*>(device);
    QVERIFY(transcodeProcess != 0);

    transcodedSize = 0;
    connect(this, SIGNAL(startTranscoding()), transcodeProcess, SLOT(startRequestData()));
    connect(this, SIGNAL(bytesSent(qint64,qint64)), transcodeProcess, SLOT(bytesSent(qint64,qint64)));
    connect(transcodeProcess, SIGNAL(sendDataToClientSignal(QByteArray)), this, SLOT(receivedTranscodedData(QByteArray)));
    connect(transcodeProcess, SIGNAL(openedSignal()), this, SLOT(transcodingOpened()));
    connect(transcodeProcess, SIGNAL(LogMessage(QString)), this, SLOT(LogMessage(QString)));
    transcodeTimer.start();
    timeToOpenTranscoding = 0;
    QVERIFY(transcodeProcess->open() == true);
    emit startTranscoding();
    transcodeProcess->waitForFinished(-1);
    qint64 duration = transcodeTimer.elapsed();

    QVERIFY(transcodeProcess->bytesAvailable() == 0);
    QVERIFY(transcodeProcess->exitCode() == 0);
    qWarning() << "DELTA" << movie.size()-transcodedSize << qAbs(double(movie.size()-transcodedSize))/movie.size();
    QVERIFY(movie.size() > transcodedSize);
    QVERIFY2(transcodedSize == 4763157848, QString("transcoded size = %1").arg(transcodedSize).toUtf8());

    qWarning() << "Transcoding opened in" << timeToOpenTranscoding << "ms and finished in" << duration << "ms.";
    QVERIFY2(timeToOpenTranscoding < 500, QString("%1").arg(timeToOpenTranscoding).toUtf8());
    QVERIFY2(duration < 450000, QString("%1").arg(duration).toUtf8());
    delete transcodeProcess;
    transcodeProcess = 0;
}

void tst_dlnavideoitem::testCase_DlnaVideoItem_MKV_Looper_MPEG2_AC3() {

    Logger log;
    log.setLevel(INF);

    DlnaVideoFile movie(&log, "/Users/doudou/Movies/Films/Action/Looper.2012.DVDRip.XviD-PTpOWeR.mkv", "host", 600);
    movie.setTranscodeFormat(MPEG2_AC3);

    QVERIFY(movie.toTranscode()==true);
    QVERIFY(movie.format() == MPEG2_AC3);
    QVERIFY(movie.getSystemName() == "/Users/doudou/Movies/Films/Action/Looper.2012.DVDRip.XviD-PTpOWeR.mkv");
    QVERIFY2(movie.bitrate()==4718800, QString("%1").arg(movie.bitrate()).toUtf8().constData());
    QVERIFY2(movie.getLengthInMilliSeconds()==6825480, QString("%1").arg(movie.getLengthInMilliSeconds()).toUtf8().constData());
    QVERIFY2(movie.size()==4026009378, QString("%1").arg(movie.size()).toUtf8().constData());
    QVERIFY(movie.framerate() == "25.000");

    QVERIFY2(movie.audioLanguages() == QStringList() << "eng", movie.audioLanguages().join(',').toUtf8());
    QVERIFY2(movie.subtitleLanguages() == QStringList() << "fre", movie.subtitleLanguages().join(',').toUtf8());

    QVERIFY(movie.getdlnaOrgOpFlags() == "10");
    QVERIFY(movie.getdlnaOrgPN() == "MPEG_PS_PAL");
    QVERIFY(movie.getDlnaContentFeatures() == "DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1");
    QVERIFY2(movie.getProtocolInfo() == "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1", movie.getProtocolInfo().toUtf8());

    QVERIFY(movie.getAlbumArt().isNull() == true);
    QVERIFY(movie.getByteAlbumArt().isNull() == true);

    QHash<QString, double> result = movie.volumeInfo(-1);
    qWarning() << result;
    QVERIFY2(result.keys().size() == 19, QString("%1").arg(result.keys().size()).toUtf8());
    QVERIFY2(result["mean_volume"] == -33.9, QString("%1").arg(result["mean_volume"]).toUtf8());
    QVERIFY2(result["max_volume"] == -0.9, QString("%1").arg(result["max_volume"]).toUtf8());

    Device *device = movie.getStream(0, 0, -1);
    transcodeProcess = qobject_cast<TranscodeProcess*>(device);
    QVERIFY(transcodeProcess != 0);

    transcodedSize = 0;
    connect(this, SIGNAL(startTranscoding()), transcodeProcess, SLOT(startRequestData()));
    connect(this, SIGNAL(bytesSent(qint64,qint64)), transcodeProcess, SLOT(bytesSent(qint64,qint64)));
    connect(transcodeProcess, SIGNAL(sendDataToClientSignal(QByteArray)), this, SLOT(receivedTranscodedData(QByteArray)));
    connect(transcodeProcess, SIGNAL(openedSignal()), this, SLOT(transcodingOpened()));
//    connect(transcodeProcess, SIGNAL(LogMessage(QString)), this, SLOT(LogMessage(QString)));
    transcodeTimer.start();
    timeToOpenTranscoding = 0;
    QVERIFY(transcodeProcess->open() == true);
    emit startTranscoding();
    transcodeProcess->waitForFinished(-1);
    qint64 duration = transcodeTimer.elapsed();

    QVERIFY(transcodeProcess->bytesAvailable() == 0);
    QVERIFY(transcodeProcess->exitCode() == 0);
    qWarning() << "DELTA" << movie.size()-transcodedSize << qAbs(double(movie.size()-transcodedSize))/movie.size();
    QVERIFY(movie.size() > transcodedSize);
    QVERIFY2(transcodedSize == 3992791752, QString("transcoded size = %1").arg(transcodedSize).toUtf8());

    QVERIFY2(timeToOpenTranscoding < 7000, QString("%1").arg(timeToOpenTranscoding).toUtf8());
    QVERIFY2(duration < 400000, QString("%1").arg(duration).toUtf8());
    qWarning() << "Transcoding opened in" << timeToOpenTranscoding << "ms and finished in" << duration << "ms.";
    delete transcodeProcess;
    transcodeProcess = 0;
}

void tst_dlnavideoitem::testCase_DlnaVideoItem_MKV_MPEG2_AC3() {

    Logger log;
    log.setLevel(INF);

    DlnaVideoFile movie(&log, "/Users/doudou/Movies/Films/District.9.2009.720p.BrRip.YIFY.mkv", "host", 600);
    movie.setTranscodeFormat(MPEG2_AC3);
    QVERIFY(movie.getSystemName() == "/Users/doudou/Movies/Films/District.9.2009.720p.BrRip.YIFY.mkv");

    QStringList properties;
    properties << "upnp:genre";
    properties << "res@size";
    properties << "res@duration";
    properties << "res@bitrate";
    properties << "res@resolution";
    properties << "res@nrAudioChannels";
    properties << "res@sampleFrequency";

    QDomDocument xml_res;
    xml_res.appendChild(movie.getXmlContentDirectory(&xml_res, properties));
    QVERIFY(xml_res.childNodes().size() == 1);
    QVERIFY(xml_res.elementsByTagName("item").size() == 1);
    QDomNode node = xml_res.elementsByTagName("item").at(0);
    QVERIFY(node.attributes().namedItem("id").nodeValue() == "");
    QVERIFY(node.attributes().namedItem("parentID").nodeValue() == "-1");
    QVERIFY(node.attributes().namedItem("restricted").nodeValue() == "true");
    QVERIFY(xml_res.elementsByTagName("dc:title").size() == 1);
    QVERIFY(xml_res.elementsByTagName("dc:title").at(0).firstChild().nodeValue() == "District.9.2009.720p.BrRip.YIFY");
    QVERIFY(xml_res.elementsByTagName("upnp:genre").size() == 1);
    QVERIFY(xml_res.elementsByTagName("upnp:genre").at(0).firstChild().nodeValue() == "");
    QVERIFY(xml_res.elementsByTagName("upnp:class").size() == 1);
    QVERIFY(xml_res.elementsByTagName("upnp:class").at(0).firstChild().nodeValue() == "object.item.videoItem");
    QVERIFY(xml_res.elementsByTagName("res").size() == 1);
    QVERIFY(xml_res.elementsByTagName("res").at(0).childNodes().size() == 1);
    QVERIFY(xml_res.elementsByTagName("res").at(0).childNodes().at(0).nodeValue() == "http://host:600/get//District.9.2009.720p.BrRip.YIFY.mkv");
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().size() == 8);
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().namedItem("protocolInfo").nodeValue() == "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1");
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().namedItem("xmlns:dlna").nodeValue() == "urn:schemas-dlna-org:metadata-1-0/");
    QVERIFY2(xml_res.elementsByTagName("res").at(0).attributes().namedItem("size").nodeValue() == "3973129325", xml_res.elementsByTagName("res").at(0).attributes().namedItem("size").nodeValue().toUtf8().constData());
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().namedItem("duration").nodeValue() == "01:52:16");
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().namedItem("bitrate").nodeValue() == "589850");
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().namedItem("resolution").nodeValue() == "1280x688");
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().namedItem("nrAudioChannels").nodeValue() == "2");
    QVERIFY(xml_res.elementsByTagName("res").at(0).attributes().namedItem("sampleFrequency").nodeValue() == "48000");
    xml_res.clear();

    QVERIFY(movie.getdlnaOrgOpFlags() == "10");
    QVERIFY(movie.getdlnaOrgPN() == "MPEG_PS_PAL");
    QVERIFY(movie.getDlnaContentFeatures() == "DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1");
    QVERIFY(movie.getProtocolInfo() == "http-get:*:video/mpeg:DLNA.ORG_PN=MPEG_PS_PAL;DLNA.ORG_OP=10;DLNA.ORG_CI=1");

    QVERIFY(movie.getAlbumArt().isNull() == true);
    QVERIFY(movie.getByteAlbumArt().isNull() == true);

    QHash<QString, double> result = movie.volumeInfo(-1);
    qWarning() << result;
    QVERIFY2(result.keys().size() == 15, QString("%1").arg(result.keys().size()).toUtf8());
    QVERIFY2(result["mean_volume"] == -28.2, QString("%1").arg(result["mean_volume"]).toUtf8());
    QVERIFY2(result["max_volume"] == -0.1, QString("%1").arg(result["max_volume"]).toUtf8());

    QVERIFY(movie.toTranscode() == true);
    QVERIFY(movie.format() == MPEG2_AC3);
    QVERIFY(movie.mimeType() == "video/mpeg");
    QVERIFY(movie.size() == 3973129325);
    QVERIFY(movie.bitrate() == 4718800);
    QVERIFY(movie.getLengthInSeconds() == 6736);
    QVERIFY2(movie.getLengthInMilliSeconds() == 6735830, QString("%1").arg(movie.getLengthInMilliSeconds()).toUtf8());
    QVERIFY(movie.samplerate() == 48000);
    QVERIFY(movie.channelCount() == 2);
    QVERIFY(movie.resolution() == "1280x688");
    QVERIFY2(movie.audioLanguages() == QStringList() << "", movie.audioLanguages().join(',').toUtf8());
    QVERIFY2(movie.subtitleLanguages() == QStringList() << "eng", movie.subtitleLanguages().join(',').toUtf8());
    QVERIFY2(movie.framerate() == "23.976", movie.framerate().toUtf8());

    // test partial transcoding (10 seconds)
    Device *device = movie.getStream(0, 0, 10);
    QVERIFY(device != 0);

    transcodeProcess = qobject_cast<TranscodeProcess*>(device);
    QVERIFY(transcodeProcess != 0);

    transcodedSize = 0;
    connect(this, SIGNAL(startTranscoding()), transcodeProcess, SLOT(startRequestData()));
    connect(this, SIGNAL(bytesSent(qint64,qint64)), transcodeProcess, SLOT(bytesSent(qint64,qint64)));
    connect(transcodeProcess, SIGNAL(sendDataToClientSignal(QByteArray)), this, SLOT(receivedTranscodedData(QByteArray)));
    connect(transcodeProcess, SIGNAL(openedSignal()), this, SLOT(transcodingOpened()));
    connect(transcodeProcess, SIGNAL(LogMessage(QString)), this, SLOT(LogMessage(QString)));
    transcodeTimer.start();
    timeToOpenTranscoding = 0;
    QVERIFY(transcodeProcess->open() == true);
    emit startTranscoding();
    transcodeProcess->waitForFinished(-1);
    qint64 duration = transcodeTimer.elapsed();

    QVERIFY(transcodeProcess->bytesAvailable() == 0);
    QVERIFY(transcodeProcess->exitCode() == 0);
    qWarning() << "DELTA" << movie.size()-transcodedSize << qAbs(double(movie.size()-transcodedSize))/movie.size();
    QVERIFY(movie.size() > transcodedSize);
    QVERIFY2(transcodedSize == 5826308, QString("transcoded size = %1").arg(transcodedSize).toUtf8());

    QVERIFY2(timeToOpenTranscoding < 10000, QString("%1").arg(timeToOpenTranscoding).toUtf8());
    QVERIFY2(duration < 10000, QString("%1").arg(duration).toUtf8());
    qWarning() << "Transcoding opened in" << timeToOpenTranscoding << "ms and finished in" << duration << "ms.";
    delete transcodeProcess;
    transcodeProcess = 0;

    // test full transcoding
    device = movie.getStream(0, 0, -1);
    QVERIFY(device != 0);

    transcodeProcess = qobject_cast<TranscodeProcess*>(device);
    QVERIFY(transcodeProcess != 0);

    transcodedSize = 0;
    connect(this, SIGNAL(startTranscoding()), transcodeProcess, SLOT(startRequestData()));
    connect(this, SIGNAL(bytesSent(qint64,qint64)), transcodeProcess, SLOT(bytesSent(qint64,qint64)));
    connect(transcodeProcess, SIGNAL(sendDataToClientSignal(QByteArray)), this, SLOT(receivedTranscodedData(QByteArray)));
    connect(transcodeProcess, SIGNAL(openedSignal()), this, SLOT(transcodingOpened()));
    connect(transcodeProcess, SIGNAL(LogMessage(QString)), this, SLOT(LogMessage(QString)));
    transcodeTimer.start();
    timeToOpenTranscoding = 0;
    QVERIFY(transcodeProcess->open()==true);
    emit startTranscoding();
    transcodeProcess->waitForFinished(-1);
    duration = transcodeTimer.elapsed();

    QVERIFY(transcodeProcess->bytesAvailable() == 0);
    QVERIFY(transcodeProcess->exitCode() == 0);
    qWarning() << "DELTA" << movie.size()-transcodedSize << qAbs(double(movie.size()-transcodedSize))/movie.size();
    QVERIFY(movie.size() > transcodedSize);
    QVERIFY2(transcodedSize == 3957535736, QString("transcoded size = %1").arg(transcodedSize).toUtf8());

    QVERIFY2(timeToOpenTranscoding < 2000, QString("%1").arg(timeToOpenTranscoding).toUtf8());
    QVERIFY2(duration < 700000, QString("%1").arg(duration).toUtf8());
    qWarning() << "Transcoding opened in" << timeToOpenTranscoding << "ms and finished in" << duration << "ms.";
    delete transcodeProcess;
    transcodeProcess = 0;
}

