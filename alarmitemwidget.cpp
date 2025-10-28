#include "alarmitemwidget.h"
#include "alarmsettingdialog.h"
#include <QLabel>
#include <QHBoxLayout>

AlarmItemWidget::AlarmItemWidget(const QTime &time, QWidget *parent)
    : QWidget(parent), alarmTime(time)
{
    // Layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);

    activeCheckBox = new QCheckBox(time.toString("hh:mm"), this);
    activeCheckBox->setChecked(true);
    layout->addWidget(activeCheckBox);

    // Time label
    QLabel *timeLabel = new QLabel(alarmTime.toString("hh:mm"), this);
    layout->addWidget(timeLabel);

    // Settings button
    settingsButton = new QPushButton("Settings", this);
    deleteButton->setMaximumWidth(60);
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
    connect(settingsButton, &QPushButton::clicked, this, [this]() {
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
        }
    });
}

QTime AlarmItemWidget::getTime() const
{
    return alarmTime;
}

bool AlarmItemWidget::isActive() const
{
    return activeCheckBox->isChecked();
}

void AlarmItemWidget::setActive(bool active)
{
    activeCheckBox->setChecked(active);
}
