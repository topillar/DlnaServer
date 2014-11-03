#include "transcodeprocess.h"

const QString TranscodeProcess::CRLF = "\r\n";

TranscodeProcess::TranscodeProcess(Logger *log, QObject *parent) :
    QProcess(parent),
    m_log(log),
    m_url(),
    m_range(0),
    timeseek_start(-1),
    timeseek_end(-1),
    m_bitrate(-1),
    m_pos(0),
    m_size(-1),
    transcodeClock(),
    killTranscodeProcess(false),
    m_paused(false),
    m_maxBufferSize(1024*1024*100),   // 100 MBytes by default when bitrate is unknown
    m_durationBuffer(10)
{
    qWarning() << "NEW" << this;

    if (!m_log)
        qWarning() << "log is not available for" << this;

    connect(m_log, SIGNAL(destroyed()), this, SLOT(_logDestroyed()));
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(dataAvailable()));
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(appendTranscodingLogMessage()));
    connect(this, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorTrancodedData(QProcess::ProcessError)));
    qRegisterMetaType<OpenMode>("OpenMode");
    connect(this, SIGNAL(openSignal(OpenMode)), this, SLOT(_open(OpenMode)));
    connect(this, SIGNAL(started()), this, SLOT(processStarted()));
    connect(this, SIGNAL(finished(int)), this, SLOT(finishedTranscodeData(int)));
    connect(this, SIGNAL(pauseSignal()), this, SLOT(_pause()));
    connect(this, SIGNAL(resumeSignal()), this, SLOT(_resume()));
}

TranscodeProcess::~TranscodeProcess()
{
    qWarning() << "DELETE" << this;

    if (state() == QProcess::Running)
    {
        killProcess();
        if (!waitForFinished(1000))
            logError("Unable to stop TranscodeProcess.");
    }
}

void TranscodeProcess::setRange(HttpRange *range)
{
    m_range = range;

//    if (m_range && m_range->getStartByte()>0)
//        seek(m_range->getStartByte());

    updateArguments();
}

void TranscodeProcess::dataAvailable()
{
    if (isLogLevel(DEBG))
        appendLog(QString("%1: received %2 bytes transcoding data."+CRLF).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")).arg(bytesAvailable()));

    if (objectName().isEmpty() && bytesAvailable() > maxBufferSize())
        setObjectName("device opened");

    // manage buffer
    if (state() == QProcess::Running) {
        if (bytesAvailable() > maxBufferSize() && !m_paused)
            pause();
    }
}

qint64 TranscodeProcess::size() const
{
    if (m_range && m_range->getLength()!=-1)
        return m_range->getLength();

    if (m_size!=-1)
        return m_size;

    return QProcess::size();
}

bool TranscodeProcess::atEnd() const
{
//    if (m_range && m_range->getEndByte()!=-1)
//        return pos() > m_range->getEndByte();

    if (bytesAvailable()>0)
        return false;

    if (m_paused or state() == QProcess::Running)
        return false;
    else
        return QProcess::atEnd();
}

qint64 TranscodeProcess::readData(char *data, qint64 maxlen)
{
    //    if (m_range && m_range->getEndByte()>0)
    //    {
    //        int bytesToRead = m_range->getEndByte() - pos() + 1;
    //        if (bytesToRead>=0 && bytesToRead<maxSize)
    //            return QFile::read(bytesToRead);
    //    }

    qint64 bytes = 0;
     if (objectName() == "device opened")
         bytes = QProcess::readData(data, maxlen);
    m_pos += bytes;

    if (m_paused && state() != QProcess::NotRunning && bytesAvailable() < (maxBufferSize()*0.5))
        resume();

    return bytes;
}

void TranscodeProcess::appendTranscodingLogMessage() {
    // incoming log message
    QByteArray msg(this->readAllStandardError());
    appendLog(msg);
}

void TranscodeProcess::errorTrancodedData(const ProcessError &error) {
    Q_UNUSED(error);

    // trancoding failed
    if (killTranscodeProcess == false) {
        // an error occured
        appendLog(QString("%2: ERROR Transcoding: %1."+CRLF).arg(errorString()).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")));
        setErrorString(errorString());
        emit errorRaised(errorString());
    }
}

void TranscodeProcess::finishedTranscodeData(const int &exitCode) {
    appendLog(QString("%2: TRANSCODE FINISHED with exitCode %1."+CRLF).arg(exitCode).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")));
    appendLog(QString("%2: TRANSCODING DONE in %1 ms."+CRLF).arg(QTime(0, 0).addMSecs(transcodeClock.elapsed()).toString("hh:mm:ss")).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")));

    if (objectName().isEmpty())
        setObjectName("device opened");

    m_paused = false;

    if (isLogLevel(DEBG))
        appendLog(QString("%1: finished transcoding, %2 remaining bytes."+CRLF).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")).arg(bytesAvailable()));

    if (isKilled() == false)
    {
        if (exitCode != 0)
        {
            // trancoding failed
            setErrorString(QString("Transcoding finished with status %1").arg(exitCode));
            emit errorRaised(QString("Transcoding finished with status %1").arg(exitCode));
            emit status("Transcoding failed.");
        }
        else
        {
            emit status("Transcoding finished.");
        }
    }
    else
    {
        emit status("Transcoding aborted.");
    }
}

void TranscodeProcess::processStarted() {
    logDebug(QString("Transcoding process %1 %2").arg(program()).arg(arguments().join(' ')));
    appendLog(program()+' ');
    appendLog(arguments().join(' ')+CRLF);

    transcodeClock.start();
}

void TranscodeProcess::killProcess() {
    if (state() != QProcess::NotRunning) {
        appendLog(QString("%1: KILL transcoding process."+CRLF).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")));
        killTranscodeProcess = true;
        kill();
    }
}

void TranscodeProcess::_pause() {
    if (!m_paused && state() != QProcess::NotRunning) {
        logDebug(QString("Pause transcoding"));
        if (isLogLevel(DEBG))
            appendLog(QString("%1: PAUSE TRANSCODING"+CRLF).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")));

        QStringList arguments;
        arguments << "-STOP" << QString("%1").arg(pid());
        QProcess pauseTrancodeProcess;
        pauseTrancodeProcess.setProgram("kill");
        pauseTrancodeProcess.setArguments(arguments);
        pauseTrancodeProcess.start();
        m_paused = pauseTrancodeProcess.waitForFinished();
    }
}

void TranscodeProcess::_resume() {
    if (m_paused && state() != QProcess::NotRunning) {
        logDebug(QString("Restart transcoding"));
        if (isLogLevel(DEBG))
            appendLog(QString("%1: RESUME TRANSCODING"+CRLF).arg(QDateTime::currentDateTime().toString("dd MMM yyyy hh:mm:ss,zzz")));

        QStringList arguments;
        arguments << "-CONT" << QString("%1").arg(pid());
        QProcess continueTrancodeProcess;
        continueTrancodeProcess.setProgram("kill");
        continueTrancodeProcess.setArguments(arguments);
        continueTrancodeProcess.start();
        m_paused = !continueTrancodeProcess.waitForFinished();
    }
}
