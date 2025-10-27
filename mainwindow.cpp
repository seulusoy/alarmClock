#include "mainwindow.h"
#include "alarmitemwidget.h"
#include "smartlineedit.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect the Set Alarm button to adding new alarms
    //connect(ui->addAlarmButton, &QPushButton::clicked, this, &MainWindow::on_addAlarmButton_clicked);

    // Make typing in alarmTimeEdit smart (auto-insert ':' after 2 digits)
    connect(ui->alarmTimeEdit, &QLineEdit::textChanged, this, &MainWindow::on_alarmTimeEdit_textChanged);

    createTrayMenu();
    createTrayIcon();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkAlarms);
    timer->start(1000); // tick every 1 second
}

MainWindow::~MainWindow()
{
    delete ui;
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
            // Trigger alarm
            QMessageBox::information(this, "Alarm",
                                     QString("Alarm for %1 triggered!").arg(alarmTime.toString("hh:mm")));

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

    QMessageBox::information(this, "Alarm Set",
                             QString("Alarm set for %1").arg(time.toString("hh:mm")));
}


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


void MainWindow::on_alarmTimeEdit_selectionChanged()
{

    ui->alarmTimeEdit->setText("");
}

