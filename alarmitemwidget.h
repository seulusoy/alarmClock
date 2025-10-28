#ifndef ALARMITEMWIDGET_H
#define ALARMITEMWIDGET_H

#include <QWidget>
#include <QTime>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>

struct Repetition {
    bool daily = false;
    bool weekdays = false;
    bool weekends = false;
    bool days[7] = {false, false, false, false, false, false, false}; // Mon → Sun
};

class AlarmItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmItemWidget(const QTime &time, QWidget *parent = nullptr);

    QTime getTime() const;
    bool isActive() const;
    void setActive(bool active);

signals:
    void deleteClicked(AlarmItemWidget *item); // emitted when delete button clicked

private:
    QTime alarmTime;
    Repetition repetition;
    QCheckBox *activeCheckBox;
    QPushButton *settingsButton;
    QPushButton *deleteButton;
};

#endif // ALARMITEMWIDGET_H
