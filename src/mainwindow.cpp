#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QScreen>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    update_remaining_time.setInterval(15);
    connect(&screenshot_timer, &QTimer::timeout, this, &MainWindow::onScreenshotTimeout);
    connect(&update_remaining_time, &QTimer::timeout, this, &MainWindow::onUpdateRemainingTime);

    connect(ui->timerPushButton, &QPushButton::pressed, this, &MainWindow::onTimerButtonPressed);
    connect(ui->makeScreenshotPushButton, &QPushButton::pressed, this, &MainWindow::onScreenshotTimeout);

    last_screenshot = takeScreenshot();
    this->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onScreenshotTimeout()
{
    last_screenshot = takeScreenshot();
    setLastScreenshot();
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    setLastScreenshot();
}

void MainWindow::setLastScreenshot()
{
    QPixmap pixmap = last_screenshot.scaled(ui->screenshotLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->screenshotLabel->setPixmap(pixmap);
}

void MainWindow::updateButton()
{
    QString str = (screenshot_timer.isActive()) ? ("Stop Timer") : ("Start Timer");
    ui->timerPushButton->setText(str);
}
