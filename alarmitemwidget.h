#ifndef ALARMITEMWIDGET_H
#define ALARMITEMWIDGET_H

#include <QWidget>
#include <QTime>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

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

    // New methods for skip-next feature
    bool isSkipNext() const;
    void setSkipNext(bool skip);

    void updateDaysLabel();
    void updateTimeLabel();

    // New method to get repetition text for display
    QString getRepetitionText() const;
    const Repetition& getRepetition() const;

    QString getSelectedSoundFile(){return selectedSoundFile;}

signals:
    void deleteClicked(AlarmItemWidget *item); // emitted when delete button clicked

private:
    QTime alarmTime;
    Repetition repetition;
    bool skipNext = false;

    QCheckBox *activeCheckBox;
    QLabel *timeLabel;
    QLabel *daysLabel;
    QPushButton *settingsButton;
    QPushButton *deleteButton;
    QPushButton *skipNextButton;

    QString selectedSoundFile;
};

#endif // ALARMITEMWIDGET_H
