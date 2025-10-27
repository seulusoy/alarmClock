#ifndef ALARMITEMWIDGET_H
#define ALARMITEMWIDGET_H

#include <QWidget>
#include <QTime>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>

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
    QCheckBox *activeCheckBox;
    QPushButton *deleteButton;
};

#endif // ALARMITEMWIDGET_H
