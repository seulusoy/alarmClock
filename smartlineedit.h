#ifndef SMARTLINEEDIT_H
#define SMARTLINEEDIT_H

#include <QLineEdit>
#include <QFocusEvent>

class SmartLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit SmartLineEdit(QWidget *parent = nullptr);

protected:
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void onTextChanged(const QString &text);
};

#endif // SMARTLINEEDIT_H
