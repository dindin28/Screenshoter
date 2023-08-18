#include "calc_thread.h"

CalcThread::CalcThread(const QPixmap& new_screenshot_, const ScreenshotCell &old_cell_, QObject *parent)
  : QThread(parent),
    new_screenshot(new_screenshot_),
    old_cell(old_cell_)
{
}

void CalcThread::run()
{
    // demostarting of working thread
    // sleep(2);
    ScreenshotCell new_cell;
    new_cell.screenshot = new_screenshot;
    if(old_cell.valid)
        new_cell.similarity_with_prev = comparePixmap(new_screenshot, old_cell.screenshot);
    else
        new_cell.similarity_with_prev = 0;
    new_cell.checksum = new_screenshot.cacheKey();
    new_cell.date = QDateTime::currentDateTime();
    new_cell.valid = true;

    emit resultReady(new_cell);
}

int CalcThread::comparePixmap(const QPixmap& left, const QPixmap& right)
{
    // TODO: check sizes or scale second
    QImage first(left.toImage());
    QImage second(right.toImage());

    int count = 0;
    const int height = first.height();
    const int width = first.width();
    for(int i = 0; i < height; ++i)
    {
        QRgb *rgb_first = reinterpret_cast<QRgb*>(first.scanLine(i));
        QRgb *rgb_second = reinterpret_cast<QRgb*>(second.scanLine(i));
        for(int j = 0; j < width; ++j)
        {
            if(rgb_first[j] == rgb_second[j])
                ++count;
        }
    }

    return (count * 100.0) / (height * width);
}