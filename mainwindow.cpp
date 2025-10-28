#include "mainwindow.h"
#include "alarmitemwidget.h"
#include "smartlineedit.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect the Set Alarm button to adding new alarms
    //connect(ui->addAlarmButton, &QPushButton::clicked, this, &MainWindow::on_addAlarmButton_clicked);

    // Make typing in alarmTimeEdit smart (auto-insert ':' after 2 digits)
    //connect(ui->alarmTimeEdit, &QLineEdit::textChanged, this, &MainWindow::SmartLineEdit::onTextChanged);

    createTrayMenu();
    createTrayIcon();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkAlarms);

    // calculate milliseconds until next minute
    QTime now = QTime::currentTime();
    int msecToNextMinute = (60 - now.second()) * 1000;
    QTimer::singleShot(msecToNextMinute, this, [this]() {
        checkAlarms();        // check immediately at start of next minute
        timer->start(60000);  // 60000 ms = 1 minute interval
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

QDateTime MainWindow::nextAlarmDateTime(AlarmItemWidget *alarm) const
{
    QTime alarmTime = alarm->getTime();
    const Repetition &rep = alarm->getRepetition(); // might need a getter for repetition
    bool skipNext = alarm->isSkipNext();

    QDate today = QDate::currentDate();
    QTime now = QTime::currentTime();

    for (int offset = 0; offset < 14; ++offset) { // look 2 weeks ahead just in case
        QDate d = today.addDays(offset);
        int dayOfWeek = d.dayOfWeek(); // 1=Mon ... 7=Sun

        bool dayMatches = false;
        if (rep.daily)
            dayMatches = true;
        else if (rep.weekdays && dayOfWeek <= 5)
            dayMatches = true;
        else if (rep.weekends && dayOfWeek >= 6)
            dayMatches = true;
        else if (rep.days[dayOfWeek - 1])
            dayMatches = true;

        if (!dayMatches) continue;

        if (offset == 0 && alarmTime <= now) continue; // skip past time today

        if (skipNext) {
            skipNext = false; // skip this one and use next matching day
            continue;
        }

        return QDateTime(d, alarmTime);
    }

    // fallback: next day + time
    return QDateTime(today.addDays(1), alarmTime);
}

void MainWindow::sortAlarms()
{
    // Create a vector of pairs: item + next datetime
    QVector<QPair<QListWidgetItem*, QDateTime>> itemsWithTime;
    for (int i = 0; i < ui->alarmListWidget->count(); ++i) {
        QListWidgetItem *item = ui->alarmListWidget->item(i);
        AlarmItemWidget *widget = qobject_cast<AlarmItemWidget*>(ui->alarmListWidget->itemWidget(item));
        if (!widget) continue;

        itemsWithTime.append(qMakePair(item, nextAlarmDateTime(widget)));
    }

    // Sort by datetime
    std::sort(itemsWithTime.begin(), itemsWithTime.end(), [](const auto &a, const auto &b) {
        return a.second < b.second;
    });

    // Reorder QListWidget
    for (int i = 0; i < itemsWithTime.size(); ++i) {
        ui->alarmListWidget->takeItem(ui->alarmListWidget->row(itemsWithTime[i].first));
        ui->alarmListWidget->insertItem(i, itemsWithTime[i].first);
    }
}

void MainWindow::checkAlarms()
{
    QTime current = QTime::currentTime();

    for (int i = 0; i < ui->alarmListWidget->count(); ++i) {
        QListWidgetItem *item = ui->alarmListWidget->item(i);
        AlarmItemWidget *widget = qobject_cast<AlarmItemWidget*>(ui->alarmListWidget->itemWidget(item));
        if (!widget) continue;

        if (!widget->isActive()) continue;

        QTime alarmTime = widget->getTime();
        if (current.hour() == alarmTime.hour() &&
            current.minute() == alarmTime.minute())
        {
            if (widget->isActive() && !widget->isSkipNext()) {
                // Trigger alarm
                QMessageBox::information(this, "Alarm",
                                     QString("Alarm for %1 triggered!").arg(alarmTime.toString("hh:mm")));

            }
            widget->setSkipNext(false); // reset after skipping
            // Set as inactive
            widget->setActive(false);
        }
    }
}


// Create the tray menu actions
void MainWindow::createTrayMenu()
{
    trayMenu = new QMenu(this);

    showAction = new QAction("Show", this);
    exitAction = new QAction("Exit", this);

    connect(showAction, &QAction::triggered, [this]() {
        this->showNormal();
        this->raise();
        this->activateWindow();
    });

    connect(exitAction, &QAction::triggered, [this]() {
        QApplication::quit();
    });

    trayMenu->addAction(showAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);
}

// Create and show the tray icon
void MainWindow::createTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);

    // Load the large PNG from resources
    QPixmap pixmap(":/assets/icons/clockIcon.png");

    // Scale to tray-friendly size (32x32) keeping aspect ratio
    QIcon icon(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Use a standard icon for now; we’ll replace with custom later
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Alarm Clock");

    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    connect(trayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            this->showNormal();
            this->raise();
            this->activateWindow();
        }
    });
}

// Intercept window close → hide instead of quit
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::on_addAlarmButton_clicked()
{
    QString text = ui->alarmTimeEdit->text();

    if (!text.contains(":")){
        text += ":00";
    }

    QTime time = QTime::fromString(text, "H:mm");
    if (!time.isValid()) {
        QMessageBox::warning(this, "Invalid time", "Please enter time in hh:mm format.");
        return;
    }

    // Add new alarm item
    QListWidgetItem *item = new QListWidgetItem(ui->alarmListWidget);
    AlarmItemWidget *widget = new AlarmItemWidget(time);
    item->setSizeHint(widget->sizeHint());
    ui->alarmListWidget->addItem(item);
    ui->alarmListWidget->setItemWidget(item, widget);

    // 🔗 Connect delete signal
    connect(widget, &AlarmItemWidget::deleteClicked, this, [=]() {
        // Find and remove this widget’s list item
        for (int i = 0; i < ui->alarmListWidget->count(); ++i) {
            QListWidgetItem *it = ui->alarmListWidget->item(i);
            if (ui->alarmListWidget->itemWidget(it) == widget) {
                delete ui->alarmListWidget->takeItem(i);
                break;
            }
        }
    });
    widget->daysLabel->setText(widget->getRepetitionText());
    sortAlarms();

    QMessageBox::information(this, "Alarm Set",
                             QString("Alarm set for %1").arg(time.toString("hh:mm")));
}

/*
void MainWindow::on_alarmTimeEdit_textChanged(const QString &arg1)
{
    QString digits = arg1;
    digits.remove(':');

    // Automatically add ':' after 2 digits
    if (digits.length() == 2 && !arg1.contains(':')) {
        ui->alarmTimeEdit->setText(digits + ":");
        ui->alarmTimeEdit->setCursorPosition(3);
    }

}
*/


void MainWindow::on_alarmTimeEdit_selectionChanged()
{

    ui->alarmTimeEdit->setText("");
}

