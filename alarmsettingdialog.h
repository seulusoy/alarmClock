#ifndef ALARMSETTINGDIALOG_H
#define ALARMSETTINGDIALOG_H

#include <QDialog>
#include <QCheckBox>

namespace Ui {
class AlarmSettingDialog;
}

class AlarmSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AlarmSettingDialog(QWidget *parent = nullptr);
    ~AlarmSettingDialog();

    void setRepetition(bool daily, bool weekdays, bool weekends, const QList<bool> &days);
    void getRepetition(bool &daily, bool &weekdays, bool &weekends, QList<bool> &days) const;

private slots:
    void onRepeatBoxChanged();
    void updateRepeatState();

private:
    Ui::AlarmSettingDialog *ui;
};

#endif // ALARMSETTINGDIALOG_H
