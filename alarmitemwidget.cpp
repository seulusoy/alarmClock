#include "alarmitemwidget.h"
#include "alarmsettingdialog.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QDate>

AlarmItemWidget::AlarmItemWidget(const QTime &time, QWidget *parent)
    : QWidget(parent), alarmTime(time)
{
    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);

    activeCheckBox = new QCheckBox(this);
    activeCheckBox->setChecked(true);
    layout->addWidget(activeCheckBox);

    // Time label
    timeLabel = new QLabel(alarmTime.toString("hh:mm"), this);
    layout->addWidget(timeLabel);

    // Days label (repetition)
    daysLabel = new QLabel(this);
    layout->addWidget(daysLabel);

    // Skip next button
    skipNextButton = new QPushButton("Skip Next", this);
    skipNextButton->setMaximumWidth(80);
    layout->addWidget(skipNextButton);

    // Settings button
    settingsButton = new QPushButton("Settings", this);
    settingsButton->setMaximumWidth(60);
    layout->addWidget(settingsButton);

    // Delete button
    deleteButton = new QPushButton("Delete", this);
    deleteButton->setMaximumWidth(60);
    layout->addWidget(deleteButton);

    layout->addStretch();

    connect(deleteButton, &QPushButton::clicked, [this]() {
        emit deleteClicked(this);
    });

    // Connect settings button
    connect(settingsButton, &QPushButton::clicked, [this]() {
        AlarmSettingDialog dlg(this);

        // Set current repetition
        dlg.setRepetition(repetition.daily, repetition.weekdays, repetition.weekends,
                          {repetition.days[0], repetition.days[1], repetition.days[2],
                           repetition.days[3], repetition.days[4], repetition.days[5],
                           repetition.days[6]});

        QList<bool> daysList;
        for (int i = 0; i < 7; ++i)
            daysList.append(repetition.days[i]);

        // Open dialog modally
        if (dlg.exec() == QDialog::Accepted) {
            dlg.getRepetition(repetition.daily, repetition.weekdays, repetition.weekends, daysList);

            // copy back to repetition.days array
            for (int i = 0; i < 7; ++i)
                repetition.days[i] = daysList[i];

            // Update daysLabel text
            daysLabel->setText(getRepetitionText());
        }
    });

    // Skip next logic
    connect(skipNextButton, &QPushButton::clicked, [this]() {
        skipNext = !skipNext;
        skipNextButton->setText(skipNext ? "Ring Next" : "Skip Next");
    });

    // Initial daysLabel
    daysLabel->setText(getRepetitionText());

}

bool AlarmItemWidget::isActive() const
{
    return activeCheckBox->isChecked();
}

void AlarmItemWidget::setActive(bool active)
{
    activeCheckBox->setChecked(active);
}

bool AlarmItemWidget::isSkipNext() const {
    return skipNext;
}

void AlarmItemWidget::setSkipNext(bool skip) {
    skipNext = skip;
    if(skipNextButton) skipNextButton->setText(skip ? "Ring Next" : "Skip Next");
}

QString AlarmItemWidget::getRepetitionText() const
{
    // Determine repetition pattern first
    QString repetitionStr;

    if (repetition.daily)
        repetitionStr = "Daily";
    else if (repetition.weekdays)
        repetitionStr = "Weekdays";
    else if (repetition.weekends)
        repetitionStr = "Weekend";
    else {
        int count = 0;
        for (int i = 0; i < 7; ++i)
            if (repetition.days[i]) ++count;

        if (count == 0)
            repetitionStr = "Once";
        else if (count == 1) {
            // find which day is selected
            const char* dayNames[] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
            for (int i = 0; i < 7; ++i)
                if (repetition.days[i])
                    repetitionStr = dayNames[i];
        }
        else
            repetitionStr = "Custom";
    }

    // Determine next day it will ring
    QString nextDayStr = "Today"; // default
    QDate today = QDate::currentDate();
    QTime now = QTime::currentTime();

    for (int offset = 0; offset < 7; ++offset) {
        QDate d = today.addDays(offset);
        int dayOfWeek = d.dayOfWeek(); // 1=Mon ... 7=Sun

        bool dayMatches = false;
        if (repetition.daily)
            dayMatches = true;
        else if (repetition.weekdays && dayOfWeek <= 5)
            dayMatches = true;
        else if (repetition.weekends && dayOfWeek >= 6)
            dayMatches = true;
        else if (repetition.days[dayOfWeek - 1])
            dayMatches = true;

        if (!dayMatches) continue;

        // If today, check time
        if (offset == 0 && alarmTime <= now) continue;

        nextDayStr = d.toString("ddd");
        break;
    }

    return nextDayStr + " " + repetitionStr;
}

QTime AlarmItemWidget::getTime() const
{
    return alarmTime;
}

const Repetition& AlarmItemWidget::getRepetition() const { return repetition; }
