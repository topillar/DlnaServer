#include "medialibrary.h"

MediaLibrary::MediaLibrary(Logger* log, QString pathname, QObject *parent) :
    QObject(parent),
    log(log),
    db(QSqlDatabase::addDatabase("QSQLITE"))
{
    db.setDatabaseName(pathname);
    if (!db.open()) {
        log->ERROR("unable to open database " + pathname);
    } else {
        QSqlQuery query;

        if (!query.exec("pragma foreign_keys = on;")) {
            log->ERROR("unable to set FOREIGN KEYS in MediaLibrary " + query.lastError().text());
        }

        if (!query.exec("create table if not exists media ("
                   "id integer primary key, "
                   "filename varchar unique, "
                   "title varchar, album integer, artist integer, genre integer, trackposition varchar, "
                   "duration integer, samplerate integer, channelcount integer, bitrate integer, resolution varchar, framerate varchar, "
                   "picture integer, "
                   "audiolanguages varchar, subtitlelanguages varchar, "
                   "format varchar, "
                   "type integer, "
                   "mime_type integer, "
                   "FOREIGN KEY(type) REFERENCES type(id), "
                   "FOREIGN KEY(mime_type) REFERENCES mime_type(id), "
                   "FOREIGN KEY(artist) REFERENCES artist(id), "
                   "FOREIGN KEY(album) REFERENCES album(id), "
                   "FOREIGN KEY(picture) REFERENCES picture(id), "
                   "FOREIGN KEY(genre) REFERENCES genre(id)"
                   ")")) {
            log->ERROR("unable to create table media in MediaLibrary " + query.lastError().text());
        }

        if (!query.exec("create table if not exists type (id integer primary key, "
                                                         "name varchar unique)")) {
            log->ERROR("unable to create table type in MediaLibrary " + query.lastError().text());
        }

        if (!query.exec("create table if not exists mime_type (id integer primary key, "
                                                              "name varchar unique)")) {
            log->ERROR("unable to create table mime_type in MediaLibrary " + query.lastError().text());
        }

        if (!query.exec("create table if not exists artist (id integer primary key, "
                                                           "name varchar unique)")) {
            log->ERROR("unable to create table artist in MediaLibrary " + query.lastError().text());
        }

        if (!query.exec("create table if not exists album (id integer primary key, "
                                                          "name varchar unique)")) {
            log->ERROR("unable to create table album in MediaLibrary " + query.lastError().text());
        }

        if (!query.exec("create table if not exists genre (id integer primary key, "
                                                          "name varchar unique)")) {
            log->ERROR("unable to create table genre in MediaLibrary " + query.lastError().text());
        }

        if (!query.exec("create table if not exists picture (id integer primary key, "
                                                            "name varchar unique)")) {
            log->ERROR("unable to create table picture in MediaLibrary " + query.lastError().text());
        }

        // update foreign keys
        foreach(QString tableName, db.tables()) {
            query.exec(QString("pragma foreign_key_list(%1);").arg(tableName));
            while (query.next()) {
                foreignKeys << query.value("from").toString();
            }
        }
    }
}

MediaLibrary::~MediaLibrary() {
    db.close();
    db.removeDatabase(db.connectionName());
}

QVariant MediaLibrary::getmetaData(QString tagName, int idMedia) {

    QSqlQuery query;
    if (foreignKeys.contains(tagName)) {
        query.exec(QString("SELECT %1.name FROM media LEFT OUTER JOIN %1 ON media.%1=%1.id WHERE media.id=%2").arg(tagName).arg(idMedia));
    } else {
        query.exec(QString("SELECT %1 FROM media WHERE id=%2").arg(tagName).arg(idMedia));
    }

    if (query.next()) {
        QVariant res = query.value(0);
        if (query.next()) {
            // at least two results have been found
            return QVariant();
        } else {
            // returns the unique result found
            return res;
        }
    } else {
        return QVariant();
    }
}

int MediaLibrary::countAllMetaData(QString tagName) {
    QSqlQuery query(QString("SELECT count(DISTINCT name) FROM %1").arg(tagName));
    if (query.next()) {
        return query.value(0).toInt();
    } else {
        return 0;
    }
}

int MediaLibrary::countMedia(QString where) {
    QSqlQuery query(QString("SELECT count(id) FROM media WHERE %1").arg(where));
    if (query.next()) {
        return query.value(0).toInt();
    } else {
        return 0;
    }
}

bool MediaLibrary::insert(QHash<QString, QVariant> data) {
    QSqlQuery query;

    QString parameters = QStringList(data.keys()).join(",");

    QStringList l_values;
    foreach(QString elt, data.keys()) {
        l_values << QString(":%1").arg(elt);
    }
    QString values = l_values.join(",");

    query.prepare(QString("INSERT INTO media (%1) VALUES (%2)").arg(parameters).arg(values));
    foreach(QString elt, data.keys()) {
        if (foreignKeys.contains(elt)) {
            // replace the value of the foreign key by its id
            int index = insertForeignKey(elt, "name", data[elt]);
            query.bindValue(QString(":%1").arg(elt), index);
            if (index == -1) {
                log->ERROR("unable to bind " + elt);
                return false;
            }
        } else {
            query.bindValue(QString(":%1").arg(elt), data[elt]);
        }
    }

    int res = query.exec();

    if (!res) {
        log->ERROR("unable to update MediaLibrary " + query.lastError().text());
    }

    return res;
}

int MediaLibrary::insertForeignKey(QString table, QString parameter, QVariant value) {

    QSqlField field(parameter, value.type());
    field.setValue(value);

    QSqlQuery query;

    int index = -1;
    query.exec(QString("SELECT id FROM %1 WHERE %2 = %3").arg(table).arg(parameter).arg(db.driver()->formatValue(field)));
    if (query.next()) {
        index = query.value(0).toInt();
    } else {
        if (!query.exec(QString("INSERT INTO %1 (%2) VALUES (%3)").arg(table).arg(parameter).arg(db.driver()->formatValue(field)))) {
            log->ERROR("unable to update MediaLibrary " + query.lastError().text());
            return -1;
        } else {
            index = query.lastInsertId().toInt();
        }
    }
    return index;
}