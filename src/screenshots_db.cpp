#include "screenshots_db.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include <QBuffer>

ScreenshotsDB::ScreenshotsDB(const QString& name)
{
    screenshot_db = QSqlDatabase::addDatabase("QSQLITE", "SQLITE");
    screenshot_db.setDatabaseName("screenshots");
}

ScreenshotsDB::~ScreenshotsDB()
{
    if(screenshot_db.isOpen())
        screenshot_db.close();
}

bool ScreenshotsDB::init()
{
    bool is_ok = screenshot_db.open();
    if(!is_ok)
        sql_error = screenshot_db.lastError();
    return is_ok;
}

bool ScreenshotsDB::createTable()
{
    QSqlQuery query(screenshot_db);
    query.prepare("CREATE TABLE IF NOT EXISTS screenshots("
                  "id                   INTEGER PRIMARY KEY AUTOINCREMENT,"
                  "screenshot           BLOB    NOT NULL,"
                  "similarity_with_prev INT,"
                  "checksum             INT,"
                  "date                 INT,"
                  "valid                INT);");
    bool is_ok = query.exec();
    if(!is_ok)
        sql_error = query.lastError();

    return is_ok;
}

bool ScreenshotsDB::select(QList<ScreenshotCell>& list)
{
    QSqlQuery query(screenshot_db);
    bool is_ok = query.exec("SELECT * FROM screenshots");
    if(is_ok)
    {
        QSqlRecord rec = query.record();
        while (query.next())
        {
            ScreenshotCell cell;

            cell.id = query.value(rec.indexOf("id")).toInt();
            cell.screenshot = toPixmap(query.value(rec.indexOf("screenshot")).toByteArray());
            cell.similarity_with_prev = query.value(rec.indexOf("similarity_with_prev")).toInt();
            cell.checksum = query.value(rec.indexOf("checksum")).toLongLong();
            cell.date = QDateTime::fromSecsSinceEpoch(query.value(rec.indexOf("date")).toLongLong());
            cell.valid = query.value(rec.indexOf("id")).toBool();

            list.push_back(cell);
        }
    }
    else
        sql_error = query.lastError();

    return is_ok;
}

bool ScreenshotsDB::insert(const ScreenshotCell& cell)
{
    QSqlQuery query(screenshot_db);
    query.prepare("INSERT INTO screenshots(screenshot, similarity_with_prev, checksum, date, valid)"
                    "VALUES (:screenshot, :similarity_with_prev, :checksum, :date, :valid);");

    query.bindValue(":screenshot", toByteArray(cell.screenshot));
    query.bindValue(":similarity_with_prev", cell.similarity_with_prev);
    query.bindValue(":checksum", cell.checksum);
    query.bindValue(":date", cell.date.toSecsSinceEpoch());
    query.bindValue(":valid", QVariant(cell.valid));

    bool is_ok = query.exec();
    if(!is_ok)
        sql_error = query.lastError();
    return is_ok;
}

QSqlError ScreenshotsDB::getLastError()
{
    return sql_error;
}

QPixmap ScreenshotsDB::toPixmap(const QByteArray& bytes)
{
    QPixmap pixmap;
    pixmap.loadFromData(bytes);
    return pixmap;
}

QByteArray ScreenshotsDB::toByteArray(const QPixmap& pixmap)
{
    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");
    return bytes;
}
