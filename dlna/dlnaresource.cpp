#include "dlnaresource.h"

DlnaResource::DlnaResource(Logger *log, QObject *parent):
    QObject(parent),
    log(log),
    id(),
    dlnaParent(0),
    updateId(1)
{
}

QString DlnaResource::getResourceId() const {
    if (getId().isNull())
        return QString();

    if (getDlnaParent() != 0)
        return getDlnaParent()->getResourceId() + '$' + getId();
    else
        return getId();
}

DlnaResource* DlnaResource::search(QString searchId, QString searchStr, QObject *parent) {
    if (getResourceId() == searchId) {
        return this;
    }
    else if (getResourceId().length() < searchId.length() and searchId.startsWith(getResourceId())) {

        int child_index = searchId.split("$").at(getResourceId().split("$").length()).toInt()-1;

        if ((child_index >= 0) && (child_index < getChildrenSize())) {
            DlnaResource *child = getChild(child_index, parent);
            if (child != 0)
                return child->search(searchId, searchStr, parent);
        }
    }

    return 0;
}

QList<DlnaResource*> DlnaResource::getDLNAResources(QString objectId, bool returnChildren, int start, int count, QString searchStr, QObject *parent) {
    QList<DlnaResource*> resources;
    DlnaResource* dlna = search(objectId, searchStr, parent);
    if (dlna != 0) {
        if (!returnChildren) {
            resources.append(dlna);
        } else {
            if (count > 0) {
                int nbChildren = dlna->getChildrenSize();
                for (int i = start; i < start + count; i++) {
                    if (i < nbChildren) {
                        DlnaResource* child = dlna->getChild(i, parent);
                        if (child != 0)
                            resources.append(child);
                    }
                }
            }
        }
    }

    return resources;
}

QString DlnaResource::getDlnaParentId() const {
    if (getDlnaParent() != 0)
        return getDlnaParent()->getResourceId();
    else
        return "-1";
}

QString DlnaResource::getStringContentDirectory(QStringList properties) {
    QDomDocument xml;
    xml.appendChild(getXmlContentDirectory(&xml, properties));

    QDomDocument res;
    QDomElement result = res.createElement("Result");
    result.appendChild(res.createTextNode(xml.toString().replace("\n", "")));
    res.appendChild(result);

    QString strRes = res.toString();

    // remove <Result> from beginning
    strRes.chop(10);

    // remove </Result> from ending
    strRes.remove(0, 8);

    return strRes;
}

QByteArray DlnaResource::getByteAlbumArt() {
    QImage picture = getAlbumArt();
    if (!picture.isNull()) {
        QByteArray result;
        QBuffer buffer(&result);
        if (buffer.open(QIODevice::WriteOnly)) {
            if (picture.save(&buffer, "JPEG")) {
                return result;
            }
        }
    }
    return QByteArray();
}

void DlnaResource::updateXmlContentDirectory(QDomDocument *xml, QDomElement *xml_obj, QStringList properties) {
    Q_UNUSED(properties);

    xml_obj->setAttribute("id", getResourceId());

    xml_obj->setAttribute("parentID", getDlnaParentId());

    QDomElement dcTitle = xml->createElement("dc:title");
    dcTitle.appendChild(xml->createTextNode(getDisplayName()));
    xml_obj->appendChild(dcTitle);

    QDomElement upnpClass = xml->createElement("upnp:class");
    upnpClass.appendChild(xml->createTextNode(getUpnpClass()));
    xml_obj->appendChild(upnpClass);

    xml_obj->setAttribute("restricted", "true");
}
