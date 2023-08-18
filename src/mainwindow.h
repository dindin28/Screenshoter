#pragma once

#include "screenshots_db.h"
#include "screenshot_cell.h"

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QPixmap takeScreenshot() const;

    double comparePixmap(const QPixmap& left, const QPixmap& right);

    bool storeCell(const ScreenshotCell& cell);
    ScreenshotCell retriveLastCell();

public slots:
    void onScreenshotTimeout();
    void onTimerButtonPressed();
    void onUpdateRemainingTime();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setLastScreenshot();
    void updateButton();

private:
    Ui::MainWindow *ui;
    ScreenshotCell last_screenshot;
    QTimer screenshot_timer;
    QTimer update_remaining_time;
    ScreenshotsDB screenshots_db;
};
