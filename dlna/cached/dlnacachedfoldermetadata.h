#ifndef DLNACACHEDFOLDERMETADATA_H
#define DLNACACHEDFOLDERMETADATA_H

#include "dlnastoragefolder.h"
#include "dlnacachedfolder.h"

#include "medialibrary.h"

class DlnaCachedFolderMetaData : public DlnaStorageFolder
{
public:
    DlnaCachedFolderMetaData(Logger* log, MediaLibrary* library, QString metaData, QString name, QString host, int port, QObject *parent = 0);

    virtual bool discoverChildren();

    virtual int getChildrenSize();

    // Any resource needs to represent the container or item with a String.
    // String to be showed in the UPNP client.
    virtual QString getName() const { return name; }

    virtual QString getSystemName() const { return name; }

    // Returns the DisplayName that is shown to the Renderer.
    virtual QString getDisplayName() { return name; }

private:
    MediaLibrary* library;
    QString metaData;
    QString name;
};

#endif // DLNACACHEDFOLDERMETADATA_H