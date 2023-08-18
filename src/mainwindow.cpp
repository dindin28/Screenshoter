#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QScreen>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , screenshots_db("screenshots.db")
{
    ui->setupUi(this);

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
        last_screenshot = last_cell;
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

    ScreenshotCell new_cell;
    new_cell.screenshot = new_screenshot;
    if(last_screenshot.valid)
        new_cell.similarity_with_prev = comparePixmap(new_screenshot, last_screenshot.screenshot);
    else
        new_cell.similarity_with_prev = 0;
    new_cell.checksum = new_screenshot.cacheKey();
    new_cell.date = QDateTime::currentDateTime();
    new_cell.valid = true;

    last_screenshot = new_cell;
    setLastScreenshot();
    storeCell(last_screenshot);
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
    QPixmap pixmap = last_screenshot.screenshot.scaled(ui->screenshotLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->screenshotLabel->setPixmap(pixmap);
    QString similarity_str = QString("Similarity with last: %1%").arg(last_screenshot.similarity_with_prev);
    ui->similarityLabel->setText(similarity_str);
    QString checksum_str = QString("Checksum of current: %1").arg(last_screenshot.checksum);
    ui->checksumLabel->setText(checksum_str);
    QString datetime_str = QString("Date/Time of current: %1").arg(last_screenshot.date.toString());
    ui->dateTimeLabel->setText(datetime_str);
}

void MainWindow::updateButton()
{
    QString str = (screenshot_timer.isActive()) ? ("Stop Timer") : ("Start Timer");
    ui->timerPushButton->setText(str);
}
