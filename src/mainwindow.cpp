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
    QPixmap new_screenshot = takeScreenshot();

    QString similarity_str = QString("Similarity with last: %1%").arg(comparePixmap(new_screenshot, last_screenshot), 0, 'f', 2);
    qDebug() << similarity_str;
    ui->similarityLabel->setText(similarity_str);

    QString checksum_str = QString("Checksum of current: %1").arg(new_screenshot.cacheKey());
    qDebug() << checksum_str;
    ui->checksumLabel->setText(checksum_str);

    last_screenshot = new_screenshot;
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

double MainWindow::comparePixmap(const QPixmap& left, const QPixmap& right)
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

    double percentage = (count * 100.0) / (height * width);
    return percentage;
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
