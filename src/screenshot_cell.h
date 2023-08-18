#pragma once

#include <QPixmap>
#include <QDateTime>
#include <iostream>

struct ScreenshotCell
{
    ScreenshotCell() = default;
    ScreenshotCell(const ScreenshotCell&) = default;
    ~ScreenshotCell() = default;

    int id;
    QPixmap screenshot;
    int similarity_with_prev;
    qint64 checksum;
    QDateTime date;
    bool valid = false;

    friend std::ostream &operator<<(std::ostream &out, const ScreenshotCell &obj)
    { 
        out << obj.id << " - " << obj.similarity_with_prev << " - " << obj.checksum
            << " - " << obj.date.toString().toStdString() << " - " << std::boolalpha << obj.valid;
         return out;            
    }
};

Q_DECLARE_METATYPE(ScreenshotCell)
