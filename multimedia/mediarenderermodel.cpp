#include "mediarenderermodel.h"

MediaRendererModel::MediaRendererModel(QObject *parent) :
    QAbstractTableModel(parent),
    mRecords(),
    mRoles()
{
    mRoles[statusRole] = "status";
    mRoles[nameRole] = "name";
    mRoles[networkAddressRole] = "networkAddress";
    mRoles[userAgentRole] = "userAgent";
}

MediaRendererModel::~MediaRendererModel()
{
    clearAll();
}

void MediaRendererModel::clearAll() {
    // remove all renderers
    qDeleteAll(mRecords);
    mRecords.clear();
}

int MediaRendererModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return mRecords.count();
}

int MediaRendererModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return mRoles.keys().length();
}

QVariant MediaRendererModel::data(const QModelIndex &index, int role) const
{
    MediaRenderer *item = 0;

    if ( index.row() >= 0 and index.row() < mRecords.size() )
        item = mRecords.at(index.row());

    if (item != 0) {
        switch (role) {
        case statusRole:
            return item->getStatus();
        case nameRole:
            return item->getName();
        case networkAddressRole:
            return item->getNetworkAddress();
        case userAgentRole:
            return item->getUserAgent();
        default:
            return QVariant::Invalid;
        }
    }

    return QVariant::Invalid;
}

void MediaRendererModel::addRendererInModel(MediaRenderer *renderer)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mRecords.append(renderer);
    endInsertRows();

    connect(renderer, SIGNAL(dataChanged(QString)), this, SLOT(rendererChanged(QString)));
}

void MediaRendererModel::rendererChanged(const QString &roleChanged)
{
    int roleChangedIndex = mRoles.key(QByteArray(roleChanged.toUtf8().constData()));
    if (mRoles.contains(roleChangedIndex)) {
        int columnIndex = roleChangedIndex-Qt::UserRole-1;
        int rendererIndex = mRecords.indexOf(qobject_cast<MediaRenderer*>(sender()));
        if (rendererIndex!=-1) {
            QVector<int> rolesChanged;
            rolesChanged.append(roleChangedIndex);
            emit dataChanged(index(rendererIndex, columnIndex), index(rendererIndex, columnIndex), rolesChanged);
        } else {
            qWarning() << "rendererChanged ERROR, sender" << sender() << "is unknown";
        }
    } else {
        qWarning() << "rendererChanged ERROR, role" << roleChanged << "is unknown";
    }
}

QVariant MediaRendererModel::get(const int &index, const int &roleIndex) const
{
    return data(this->index(index, 0), Qt::UserRole+1+roleIndex);
}

MediaRenderer* MediaRendererModel::getFromIp(const QString &ip) const {
    foreach(MediaRenderer* renderer, mRecords) {
        if (renderer->getNetworkAddress() == ip) {
            return renderer;
        }
    }

    return 0;
}

void MediaRendererModel::serving(const QString &ip, const QString &mediaName) {
    MediaRenderer* renderer = getFromIp(ip);
    if (renderer != 0)
        renderer->setStatus(QString("Serving %1").arg(mediaName));
}

void MediaRendererModel::stopServing(const QString &ip) {
    MediaRenderer* renderer = getFromIp(ip);
    if (renderer != 0)
        renderer->setStatus("standby");
}
