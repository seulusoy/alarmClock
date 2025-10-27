#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <QList>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void checkAlarms();

protected:
    void closeEvent(QCloseEvent *event) override; // to minimize instead of closing


private slots:
    void on_addAlarmButton_clicked();
    void on_alarmTimeEdit_textChanged(const QString &arg1);

    void on_alarmTimeEdit_selectionChanged();

private:
    Ui::MainWindow *ui;

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *showAction;
    QAction *exitAction;

    QList<QTime> alarms;   // store multiple alarm times

    QTime alarmTime;       // stores the user-set alarm
    bool alarmSet = false; // is an alarm active
    QTimer *timer;         // checks the clock every second

    void createTrayIcon();
    void createTrayMenu();
};
#endif // MAINWINDOW_H
