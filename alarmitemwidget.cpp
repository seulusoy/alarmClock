#include "alarmitemwidget.h"

AlarmItemWidget::AlarmItemWidget(const QTime &time, QWidget *parent)
    : QWidget(parent), alarmTime(time)
{
    activeCheckBox = new QCheckBox(time.toString("hh:mm"), this);
    activeCheckBox->setChecked(true);

    deleteButton = new QPushButton("Delete", this);
    deleteButton->setMaximumWidth(60);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(activeCheckBox);
    layout->addStretch();
    layout->addWidget(deleteButton);
    layout->setContentsMargins(0,0,0,0);

    connect(deleteButton, &QPushButton::clicked, [this]() {
        emit deleteClicked(this);
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
