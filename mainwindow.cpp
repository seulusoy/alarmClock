#include "mainwindow.h"
#include "alarmitemwidget.h"
#include "smartlineedit.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <QDateTime>
#include <QSoundEffect>
#include <QUrl>
#include <QFileInfo>

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
    //connect(timer, &QTimer::timeout, this, &MainWindow::checkAlarms());

    connect(timer, &QTimer::timeout, this, [this]() {
        checkAlarms();
    });

    // calculate milliseconds until next minute
    QTime now = QTime::currentTime();
    int msecToNextMinute = (60 - now.second()) * 1000;
    QTimer::singleShot(msecToNextMinute, this, [this]() {
        checkAlarms();        // check immediately at start of next minute
        timer->start(60000);  // 60000 ms = 1 minute interval
    });

    connect(ui->triggerAllButton, &QPushButton::clicked, this, [this]() {
        checkAlarms(true);
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
/*
void MainWindow::sortAlarms()
{

    struct AlarmEntry {
        QListWidgetItem* item;
        AlarmItemWidget* widget;
        QDateTime nextTime;
    };
    QVector<AlarmEntry> alarms;

    while(ui->alarmListWidget->count()>0){
        QListWidgetItem *item = ui->alarmListWidget->takeItem(0);
        AlarmItemWidget* widget = qobject_cast<AlarmItemWidget*>(ui->alarmListWidget->itemWidget(item));
        alarms.append({item, widget, nextAlarmDateTime(widget)});
    }

    // Step 2: Sort by next alarm time
    std::sort(alarms.begin(), alarms.end(), [](const AlarmEntry &a, const AlarmEntry &b) {
        return a.nextTime < b.nextTime;
    });

    // Step 3: Remove all items from the list WITHOUT deleting them
    for (int i = ui->alarmListWidget->count() - 1; i >= 0; --i)
        ui->alarmListWidget->takeItem(i);

    // Step 4: Reinsert items and reattach widgets
    for (const auto &entry : alarms) {
        AlarmItemWidget* widget = new AlarmItemWidget(time, nullptr);
        ui->alarmListWidget->addItem(entry.item);
        ui->alarmListWidget->setItemWidget(entry.item, entry.widget);
        entry.item->setSizeHint(entry.widget->sizeHint());
    }
}
*/

void MainWindow::checkAlarms(bool forceTrigger)
{
    QTime current = QTime::currentTime();

    for (int i = 0; i < ui->alarmListWidget->count(); ++i) {
        QListWidgetItem *item = ui->alarmListWidget->item(i);
        AlarmItemWidget *widget = qobject_cast<AlarmItemWidget*>(ui->alarmListWidget->itemWidget(item));
        if (!widget) continue;

        if (!widget->isActive()) continue;

        QTime alarmTime = widget->getTime();

        bool shouldTrigger =
            forceTrigger ||
            (current.hour() == alarmTime.hour() && current.minute() == alarmTime.minute());

        if (shouldTrigger) {

            if (widget->isSkipNext()){
                widget->setSkipNext(false);
                continue;
            }

            // widget->setSkipNext(false); // reset after skipping
            // Set as inactive
            if (!widget->getRepetition().daily &&
                !widget->getRepetition().weekdays &&
                !widget->getRepetition().weekends &&
                std::none_of(std::begin(widget->getRepetition().days),
                             std::end(widget->getRepetition().days),
                             [](bool d){return d; })) {
                widget->setActive(false);
            }

            QString soundFile = widget->getSelectedSoundFile();

            if (soundFile.isEmpty()) {
                // 🔔 No sound selected → only show message
                QMessageBox::information(this, "Alarm",
                                         QString("Alarm for %1 triggered!")
                                             .arg(alarmTime.toString("hh:mm")));
            } else {
                // 🎵 Play selected sound
                QSoundEffect *effect = new QSoundEffect(this);
                effect->setSource(QUrl::fromLocalFile(soundFile));
                effect->setLoopCount(QSoundEffect::Infinite); // or 1 if you want single play
                effect->setVolume(0.8f);
                effect->play();

                // Optional message showing which sound is playing
                QMessageBox::information(this, "Alarm",
                                         QString("Alarm for %1 triggered!\nPlaying: %2")
                                             .arg(alarmTime.toString("hh:mm"))
                                             .arg(QFileInfo(soundFile).fileName()));

                // Stop the sound when the message box is closed
                effect->stop();
                effect->deleteLater();
            }
            if (forceTrigger){
                forceTrigger = false;
            }
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
    QString text = ui->alarmTimeEdit->text().trimmed();
    if (!text.contains(":")) text += ":00";

    QTime time = QTime::fromString(text, "H:mm");
    if (!time.isValid()) {
        QMessageBox::warning(this, "Invalid Time", "Please enter time in hh:mm format.");
        return;
    }

    // Step 1: Create widget with no parent (list will take ownership)
    AlarmItemWidget* widget = new AlarmItemWidget(time, nullptr);

    // Step 2: Create list item
    QListWidgetItem* item = new QListWidgetItem(ui->alarmListWidget);
    item->setSizeHint(widget->sizeHint());
    ui->alarmListWidget->addItem(item);
    ui->alarmListWidget->setItemWidget(item, widget);

    // Step 3: Connect delete button
    connect(widget, &AlarmItemWidget::deleteClicked, this, [this, widget]() {
        for (int i = 0; i < ui->alarmListWidget->count(); ++i) {
            QListWidgetItem* it = ui->alarmListWidget->item(i);
            if (ui->alarmListWidget->itemWidget(it) == widget) {
                delete ui->alarmListWidget->takeItem(i);
                break;
            }
        }
    });

    // Step 4: Update repetition label
    widget->updateDaysLabel();

    // Step 5: Sort all alarms
    //sortAlarms();

    // Step 6: Notify user
    QMessageBox::information(this, "Alarm Set",
                             QString("Alarm set for %1").arg(time.toString("hh:mm")));

    // Step 7: Clear input
    ui->alarmTimeEdit->clear();
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

