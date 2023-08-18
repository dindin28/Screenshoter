#pragma once

#include "screenshots_db.h"
#include "screenshot_cell.h"

#include <QMainWindow>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>

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

    bool storeCell(const ScreenshotCell& cell);
    ScreenshotCell retriveLastCell();

public slots:
    void onScreenshotTimeout();
    void onTimerButtonPressed();
    void onUpdateRemainingTime();
    void onCalcThreadFinished(const ScreenshotCell &new_cell);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setLastScreenshot();
    void updateButton();

private:
    Ui::MainWindow *ui;
    ; // mutex on access
    QTimer screenshot_timer;
    QTimer update_remaining_time;
    ScreenshotsDB screenshots_db;


    struct ScreenshotCellGuard
    {
    public:
        ScreenshotCell get()
        {
            QMutexLocker locker(&mutex);
            return cell;
        }
        void set(const ScreenshotCell& new_cell)
        {
            QMutexLocker locker(&mutex);
            cell = new_cell;
        }
    private:
        ScreenshotCell cell;
        QMutex mutex;
    } last_screenshot;
};
