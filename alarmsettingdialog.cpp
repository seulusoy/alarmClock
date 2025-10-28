#include "alarmsettingdialog.h"
#include "ui_alarmsettingdialog.h"
#include <QSignalBlocker>

AlarmSettingDialog::AlarmSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AlarmSettingDialog)
{
    ui->setupUi(this);

    // Connect checkboxes
    QList<QCheckBox*> boxes = {
        ui->checkDaily, ui->checkWeekdays, ui->checkWeekend,
        ui->checkMon, ui->checkTue, ui->checkWed,
        ui->checkThu, ui->checkFri, ui->checkSat, ui->checkSun
    };

    for (auto *box : boxes)
        connect(box, &QCheckBox::toggled, this, &AlarmSettingDialog::onRepeatBoxChanged);
}

AlarmSettingDialog::~AlarmSettingDialog()
{
    delete ui;
}

void AlarmSettingDialog::onRepeatBoxChanged()
{
    QSignalBlocker b1(ui->checkDaily);
    QSignalBlocker b2(ui->checkWeekdays);
    QSignalBlocker b3(ui->checkWeekend);

    QSignalBlocker b4(ui->checkMon);
    QSignalBlocker b5(ui->checkTue);
    QSignalBlocker b6(ui->checkWed);
    QSignalBlocker b7(ui->checkThu);
    QSignalBlocker b8(ui->checkFri);
    QSignalBlocker b9(ui->checkSat);
    QSignalBlocker b10(ui->checkSun);

    QObject *src = sender();
    if (src == ui->checkDaily) {
        bool s = ui->checkDaily->isChecked();
        ui->checkMon->setChecked(s);
        ui->checkTue->setChecked(s);
        ui->checkWed->setChecked(s);
        ui->checkThu->setChecked(s);
        ui->checkFri->setChecked(s);
        ui->checkSat->setChecked(s);
        ui->checkSun->setChecked(s);
    }
    else if (src == ui->checkWeekdays) {
        bool s = ui->checkWeekdays->isChecked();
        ui->checkMon->setChecked(s);
        ui->checkTue->setChecked(s);
        ui->checkWed->setChecked(s);
        ui->checkThu->setChecked(s);
        ui->checkFri->setChecked(s);
    }
    else if (src == ui->checkWeekend) {
        bool s = ui->checkWeekend->isChecked();
        ui->checkSat->setChecked(s);
        ui->checkSun->setChecked(s);
    }

    updateRepeatState();
}

void AlarmSettingDialog::updateRepeatState()
{
    bool allDays = ui->checkMon->isChecked() && ui->checkTue->isChecked() && ui->checkWed->isChecked() &&
                   ui->checkThu->isChecked() && ui->checkFri->isChecked() &&
                   ui->checkSat->isChecked() && ui->checkSun->isChecked();

    bool weekdays = ui->checkMon->isChecked() && ui->checkTue->isChecked() && ui->checkWed->isChecked() &&
                    ui->checkThu->isChecked() && ui->checkFri->isChecked();

    bool weekends = ui->checkSat->isChecked() && ui->checkSun->isChecked();

    ui->checkDaily->setChecked(allDays);
    ui->checkWeekdays->setChecked(weekdays);
    ui->checkWeekend->setChecked(weekends);
}

void AlarmSettingDialog::setRepetition(bool daily, bool weekdays, bool weekends, const QList<bool> &days)
{
    ui->checkDaily->setChecked(daily);
    ui->checkWeekdays->setChecked(weekdays);
    ui->checkWeekend->setChecked(weekends);

    if (days.size() == 7) {
        ui->checkMon->setChecked(days[0]);
        ui->checkTue->setChecked(days[1]);
        ui->checkWed->setChecked(days[2]);
        ui->checkThu->setChecked(days[3]);
        ui->checkFri->setChecked(days[4]);
        ui->checkSat->setChecked(days[5]);
        ui->checkSun->setChecked(days[6]);
    }
}

void AlarmSettingDialog::getRepetition(bool &daily, bool &weekdays, bool &weekends, QList<bool> &days) const
{
    daily = ui->checkDaily->isChecked();
    weekdays = ui->checkWeekdays->isChecked();
    weekends = ui->checkWeekend->isChecked();

    days.clear();
    days.append(ui->checkMon->isChecked());
    days.append(ui->checkTue->isChecked());
    days.append(ui->checkWed->isChecked());
    days.append(ui->checkThu->isChecked());
    days.append(ui->checkFri->isChecked());
    days.append(ui->checkSat->isChecked());
    days.append(ui->checkSun->isChecked());
}
