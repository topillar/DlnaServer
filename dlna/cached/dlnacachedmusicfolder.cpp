#include "dlnacachedmusicfolder.h"

DlnaCachedMusicFolder::DlnaCachedMusicFolder(Logger* log, MediaLibrary *library, QString host, int port, int idType, QObject *parent):
    DlnaStorageFolder(log, host, port, parent),
    children()
{
    DlnaResource* child = 0;

    child = new DlnaCachedFolderMetaData(log, library, idType, "artist", "Artist",
                                         host, port,
                                         QString("album, trackposition"), QString("ASC"),
                                         QString(), this);
    child->setId(QString("%1").arg(children.length()+1));
    children.append(child);
    child->setDlnaParent(this);

    child = new DlnaCachedFolderMetaData(log, library, idType, "album", "Album",
                                         host, port,
                                         QString("trackposition"), QString("ASC"),
                                         QString(), this);
    child->setId(QString("%1").arg(children.length()+1));
    children.append(child);
    child->setDlnaParent(this);

    child = new DlnaCachedFolderMetaData(log, library, idType, "genre", "Genre",
                                         host, port,
                                         QString("album, trackposition"), QString("ASC"),
                                         QString(), this);
    child->setId(QString("%1").arg(children.length()+1));
    children.append(child);
    child->setDlnaParent(this);
}
