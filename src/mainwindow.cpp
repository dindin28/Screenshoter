#include "mainwindow.h"
#include "calc_thread.h"
#include "./ui_mainwindow.h"

#include <QScreen>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , screenshots_db("screenshots")
{
    ui->setupUi(this);

    qRegisterMetaType<ScreenshotCell>();

    if(!screenshots_db.init())
        qDebug() << "Couldn't init:" << screenshots_db.getLastError().text();
    if(!screenshots_db.createTable())
        qDebug() << "Cannot create table:" << screenshots_db.getLastError().text();

    update_remaining_time.setInterval(15);
    connect(&screenshot_timer, &QTimer::timeout, this, &MainWindow::onScreenshotTimeout);
    connect(&update_remaining_time, &QTimer::timeout, this, &MainWindow::onUpdateRemainingTime);

    connect(ui->timerPushButton, &QPushButton::pressed, this, &MainWindow::onTimerButtonPressed);
    connect(ui->makeScreenshotPushButton, &QPushButton::pressed, this, &MainWindow::onScreenshotTimeout);

    ScreenshotCell last_cell = retriveLastCell();
    if(last_cell.valid)
    {
        last_screenshot.set(last_cell);
        setLastScreenshot();
    }
    else
        onScreenshotTimeout();

    this->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onScreenshotTimeout()
{
    const QPixmap new_screenshot = takeScreenshot();

    CalcThread *calc_thread = new CalcThread(new_screenshot, last_screenshot.get(), this);
    connect(calc_thread, &CalcThread::resultReady, this, &MainWindow::onCalcThreadFinished);
    connect(calc_thread, &CalcThread::finished, calc_thread, &QObject::deleteLater);
    calc_thread->start();
}

void MainWindow::onCalcThreadFinished(const ScreenshotCell &new_cell)
{
    qDebug() << "Thread finished";
    last_screenshot.set(new_cell);
    setLastScreenshot();
    storeCell(last_screenshot.get());
}

void MainWindow::onUpdateRemainingTime()
{
    QString str = QString("Remaining time: %1 s").arg(screenshot_timer.remainingTime() / 1000);
    ui->remainingTimeLabel->setText(str);
}

void MainWindow::onTimerButtonPressed()
{
    if(screenshot_timer.isActive())
    {
        update_remaining_time.stop();
        screenshot_timer.stop();
    }
    else
    {
        update_remaining_time.start();
        screenshot_timer.start(ui->timerSpinBox->value() * 1000);
    }
    updateButton();
}

QPixmap MainWindow::takeScreenshot() const
{
    QScreen* screen = QApplication::primaryScreen();
    return screen->grabWindow(0);
}

bool MainWindow::storeCell(const ScreenshotCell& cell)
{
    if(!screenshots_db.insert(cell))
    {
        qDebug() << "Couldn't insert:" << screenshots_db.getLastError().text();
        return false;
    }
    return true;
}

ScreenshotCell MainWindow::retriveLastCell()
{
    QList<ScreenshotCell> list;
    if(!screenshots_db.select(list))
        qDebug() << "Couldn't select:" << screenshots_db.getLastError().text();

    if(!list.empty())
    {
        return list.last();
    }
    return ScreenshotCell();
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    setLastScreenshot();
}

void MainWindow::setLastScreenshot()
{
    ScreenshotCell screenshot = last_screenshot.get();
    if(screenshot.valid)
    {
        QPixmap pixmap = screenshot.screenshot.scaled(ui->screenshotLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->screenshotLabel->setPixmap(pixmap);

        QString similarity_str = QString("Similarity with last: %1%").arg(last_screenshot.get().similarity_with_prev);
        ui->similarityLabel->setText(similarity_str);
        QString checksum_str = QString("Checksum of current: %1").arg(last_screenshot.get().checksum);
        ui->checksumLabel->setText(checksum_str);
        QString datetime_str = QString("Date/Time of current: %1").arg(last_screenshot.get().date.toString());
        ui->dateTimeLabel->setText(datetime_str);
    }
}

void MainWindow::updateButton()
{
    QString str = (screenshot_timer.isActive()) ? ("Stop Timer") : ("Start Timer");
    ui->timerPushButton->setText(str);
}
