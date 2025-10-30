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

    QString getSelectedSound() const { return selectedSound; }

private slots:
    void onRepeatBoxChanged();
    void updateRepeatState();

private:
    Ui::AlarmSettingDialog *ui;
    void populateSoundComboBox();
    QString selectedSound;
};

#endif // ALARMSETTINGDIALOG_H
