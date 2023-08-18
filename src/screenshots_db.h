#pragma once

#include "screenshot_cell.h"

#include <QSqlDatabase>
#include <QPixmap>
#include <QList>
#include <QSqlError>

class ScreenshotsDB : public QSqlDatabase
{
public:
    ScreenshotsDB(const QString& name);
    ~ScreenshotsDB();

    bool init();
    bool createTable();
    bool insert(const ScreenshotCell& cell);
    bool select(QList<ScreenshotCell>& list);

    QSqlError getLastError();

private:
    QPixmap toPixmap(const QByteArray& bytes);
    QByteArray toByteArray(const QPixmap& pixmap);

private:
    QSqlDatabase screenshot_db;
    QSqlError sql_error;
};
