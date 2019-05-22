#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

#include "paths.h"
#include "imagecache.h"

static const QString DbName = "cache_db";
static const QString DbFileName = "cache.sqlite";

enum class Column
{
    ID,
    SvgPath,
    PngPath,
    Backend,
    Hash,
};

static void initCacheDb()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE", DbName);
    db.setDatabaseName(Paths::workDir() + '/' + DbFileName);
    db.open();

    QSqlQuery query(db);
    query.exec("CREATE TABLE Cache ("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
        "SvgPath TEXT,"
        "PngPath TEXT,"
        "Backend TEXT,"
        "Hash TEXT"
    ");");
}

static void openCacheDb()
{
    if (!QFile::exists(DbFileName)) {
        initCacheDb();
    } else {
        auto db = QSqlDatabase::addDatabase("QSQLITE", DbName);
        db.setDatabaseName(DbFileName);
        db.open();
    }
}

static QString fileMd5Sum(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        throw QString("failed to open %1").arg(path);
    }

    return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
}

ImageCache::ImageCache()
{
    openCacheDb();
}

ImageCache::~ImageCache()
{
    auto db = QSqlDatabase::database(DbName);
    db.close();
    db = QSqlDatabase();
    db.removeDatabase(DbName);
}

QImage ImageCache::getImage(const Backend backend, const QString &svgPath)
{
    auto db = QSqlDatabase::database(DbName);
    QSqlQuery query(db);
    query.exec(QString("SELECT * FROM Cache WHERE SvgPath='%1' AND Backend='%2';")
                    .arg(svgPath).arg(backendToString(backend)));

    if (query.next()) {
        const auto id = query.value((int)Column::ID).toString();
        const auto hash = fileMd5Sum(svgPath);
        const auto dbHash = query.value((int)Column::Hash).toByteArray();
        const auto pngPath = query.value((int)Column::PngPath).toString();

        if (dbHash == hash) {
            return QImage(pngPath);
        } else {
            // Cache mismatch.

            // Remove cached image.
            QFile::remove(pngPath);

            // Remove from DB.
            QSqlQuery query2(db);
            query2.exec(QString("DELETE FROM Cache WHERE ID='%1';").arg(id));
        }
    }

    return QImage();
}

void ImageCache::setImage(const Backend backend, const QString &svgPath, const QImage &img)
{
    const auto imagesDir = Paths::workDir() + "/images";

    if (!QDir().exists(imagesDir)) {
        QDir().mkdir(imagesDir);
    }

    const auto pngPath = imagesDir + '/' + QUuid::createUuid().toRfc4122().toHex() + ".png";

    img.save(pngPath);

    auto db = QSqlDatabase::database(DbName);
    QSqlQuery query(db);
    query.prepare("INSERT INTO Cache "
                  "       ( SvgPath,  PngPath,  Backend,  Hash) "
                  "VALUES (:SvgPath, :PngPath, :Backend, :Hash);");
    query.bindValue(":SvgPath", svgPath);
    query.bindValue(":PngPath", pngPath);
    query.bindValue(":Backend", backendToString(backend));
    query.bindValue(":Hash", fileMd5Sum(svgPath));
    query.exec();
}
