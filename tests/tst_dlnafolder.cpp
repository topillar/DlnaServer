#include "tst_dlnafolder.h"

tst_dlnafolder::tst_dlnafolder(QObject *parent) :
    QObject(parent)
{
}


void tst_dlnafolder::testCase_DlnaFolder()
{
    Logger log;
    DlnaFolder music(&log, "/Users/doudou/Music/iTunes/iTunes Media/Music", "host", 300);
    QVERIFY(music.getId() == "");
    QVERIFY(music.getName() == "Music");
    QVERIFY(music.getSystemName() == "/Users/doudou/Music/iTunes/iTunes Media/Music");
    QVERIFY(music.getDisplayName() == "Music");
    QVERIFY(music.getParent() == 0);
    QVERIFY(music.getParentId() == "-1");
    QVERIFY(music.getResourceId() == "");
    QVERIFY(music.isFolder() == true);
    QVERIFY(music.isDiscovered() == false);
    QVERIFY(music.getUpdateId() == 1);
    QVERIFY2(music.getChildren().isEmpty() == false, QString("%1").arg(music.getChildren().isEmpty()).toUtf8());
    QVERIFY(music.getChildrenSize() == 608);
    QVERIFY(music.isDiscovered() == true);

    music.setId("0$1");
    QVERIFY(music.getId() == "0$1");

    QList<DlnaResource*> list_found;
    list_found = music.getDLNAResources("0$1", false, 0, 10, "");
    QVERIFY(list_found.isEmpty() == false);
    QVERIFY(list_found.size() == 1);

    list_found = music.getDLNAResources("0$1", true, 0, 10, "");
    QVERIFY(list_found.isEmpty() == false);
    QVERIFY2(list_found.size() == 10, QString("%1").arg(list_found.size()).toUtf8());
    QVERIFY(list_found.at(0)->getResourceId() == "0$1$1");
    QVERIFY(list_found.at(0)->getSystemName() == "/Users/doudou/Music/iTunes/iTunes Media/Music/-M-");

    list_found = music.getDLNAResources("0$1$1", true, 0, 10, "");
    QVERIFY(list_found.isEmpty() == false);
    QVERIFY2(list_found.size() == 2, QString("%1").arg(list_found.size()).toUtf8());
    QVERIFY(list_found.at(0)->getResourceId() == "0$1$1$1");
    QVERIFY(list_found.at(0)->getSystemName() == "/Users/doudou/Music/iTunes/iTunes Media/Music/-M-/Je dis aime");

    list_found = music.getDLNAResources(QString(), true, 0, 10, "");
    QVERIFY(list_found.isEmpty() == true);

    list_found = music.getDLNAResources("", true, 0, 10, "");
    QVERIFY(list_found.isEmpty() == true);

    list_found = music.getDLNAResources("10000", true, 0, 10, "");
    QVERIFY(list_found.isEmpty() == true);
}

int tst_dlnafolder::parseFolder(QString resourceId, DlnaResource *resource) {
    QElapsedTimer timer;
    int elapsed = 0;

    QList<DlnaResource*> l_child;
    for (int i = 0; i<3; i++) {

        timer.restart();
        l_child = resource->getDLNAResources(resourceId, true, 0, 608, "");
        foreach(DlnaResource* child, l_child) {
            child->getStringContentDirectory(QStringList("*"));
        }
        int tmp = timer.elapsed();
        if (tmp > elapsed) {
            elapsed = tmp;
        }
    }
    return elapsed;
}

void tst_dlnafolder::testCase_PerformanceAllArtists() {
    Logger log;
    DlnaFolder music(&log, "/Users/doudou/Music/iTunes/iTunes Media/Music", "host", 300);
    QVERIFY(music.getId() == "");
    QVERIFY(music.getName() == "Music");
    QVERIFY(music.getSystemName() == "/Users/doudou/Music/iTunes/iTunes Media/Music");
    QVERIFY(music.getDisplayName() == "Music");
    QVERIFY(music.getParent() == 0);
    QVERIFY(music.getParentId() == "-1");
    QVERIFY(music.getResourceId() == "");
    QVERIFY(music.isFolder() == true);
    QVERIFY(music.isDiscovered() == false);
    QVERIFY(music.getUpdateId() == 1);

    music.setId("0$1");
    QVERIFY(music.getId() == "0$1");

    int duration = parseFolder("0$1", &music);
    qWarning() << "PERFO" << duration << music.getSystemName() << music.getChildrenSize() << "children";
    QVERIFY2(duration < 200, QString("Parse all artists in %1 ms").arg(duration).toUtf8());
    QVERIFY(music.getChildrenSize() == 608);
}

void tst_dlnafolder::testCase_PerformanceAllAlbums() {
    Logger log;
    DlnaFolder music(&log, "/Users/doudou/Music/iTunes/iTunes Media/Music", "host", 300);
    QVERIFY(music.getId() == "");
    QVERIFY(music.getName() == "Music");
    QVERIFY(music.getSystemName() == "/Users/doudou/Music/iTunes/iTunes Media/Music");
    QVERIFY(music.getDisplayName() == "Music");
    QVERIFY(music.getParent() == 0);
    QVERIFY(music.getParentId() == "-1");
    QVERIFY(music.getResourceId() == "");
    QVERIFY(music.isFolder() == true);
    QVERIFY(music.isDiscovered() == false);
    QVERIFY(music.getUpdateId() == 1);

    music.setId("0$1");
    QVERIFY(music.getId() == "0$1");

    int max = 0;
    foreach(DlnaResource* artist, music.getChildren()) {
        int elapsed;
        elapsed = parseFolder(artist->getResourceId(), &music);
        if (elapsed > max) {
            max = elapsed;
            qWarning() << "PERFO" << elapsed << artist->getResourceId() << artist->getSystemName() << artist->getChildrenSize() << "children";
        }
    }
    QVERIFY2(max < 310, QString("Parse all albums by artist in %1 ms").arg(max).toUtf8());
}

void tst_dlnafolder::testCase_PerformanceAllTracks() {
    Logger log;
    DlnaFolder music(&log, "/Users/doudou/Music/iTunes/iTunes Media/Music", "host", 300);
    QVERIFY(music.getId() == "");
    QVERIFY(music.getName() == "Music");
    QVERIFY(music.getSystemName() == "/Users/doudou/Music/iTunes/iTunes Media/Music");
    QVERIFY(music.getDisplayName() == "Music");
    QVERIFY(music.getParent() == 0);
    QVERIFY(music.getParentId() == "-1");
    QVERIFY(music.getResourceId() == "");
    QVERIFY(music.isFolder() == true);
    QVERIFY(music.isDiscovered() == false);
    QVERIFY(music.getUpdateId() == 1);

    music.setId("0$1");
    QVERIFY(music.getId() == "0$1");

    int max = 0;
    foreach(DlnaResource* artist, music.getChildren()) {
        foreach(DlnaResource* album, artist->getChildren()) {
            int elapsed;
            elapsed = parseFolder(album->getResourceId(), &music);
            if (elapsed > max) {
                max = elapsed;
                qWarning() << "PERFO" << elapsed << album->getResourceId() << album->getSystemName() << album->getChildrenSize() << "children";
            }
        }
    }
    QVERIFY2(max < 6000, QString("Parse all tracks by album in %1 ms").arg(max).toUtf8());
}
