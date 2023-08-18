#pragma once

#include "screenshot_cell.h"

#include <QThread>

class CalcThread : public QThread
{
    Q_OBJECT

public:
    explicit CalcThread(const QPixmap& new_screenshot, const ScreenshotCell &old_cell, QObject *parent = 0);

signals:
    void resultReady(const ScreenshotCell &new_cell);

protected:
    void run() override;

private:
    int comparePixmap(const QPixmap& left, const QPixmap& right);

private:
    QPixmap new_screenshot;
    ScreenshotCell old_cell;
};
