#include "smartlineedit.h"

SmartLineEdit::SmartLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    setText("hh:mm");
    setStyleSheet("color: black;");
    setMaxLength(5); // hh:mm (including colon)
    setFocusPolicy(Qt::StrongFocus);

    connect(this, &QLineEdit::textChanged, this, &SmartLineEdit::onTextChanged);
}

void SmartLineEdit::focusInEvent(QFocusEvent *event)
{
    QLineEdit::focusInEvent(event);
    if (text() == "hh:mm") {
        setText("");
    }
    setStyleSheet("color: black;");
}

void SmartLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);

    QString digits = text();
    digits.remove(':');

    if (digits.isEmpty()) {
        setText("hh:mm");
        setStyleSheet("color: black;");
    }
}

// Main logic for handling user input
void SmartLineEdit::onTextChanged(const QString &input)
{
    QString digits = input;
    if (digits=="hh:mm"){
        return;
    }
    digits.remove(':');

    // Temporarily block signals to prevent recursion
    QSignalBlocker blocker(this);

    if (digits.isEmpty())
        return;

    if (digits.length() == 1 || digits.length() == 2) {
        int h = digits.toInt();
        /*if (h > 23) {
            digits = "23"; // clamp
        }*/
        //setText(QString("%1").arg(digits.rightJustified(2, '0')));
        setText(QString("%1").arg(digits));
        setCursorPosition(text().length());
    }
    else if (digits.length() == 3) {
        int h = digits.left(1).toInt();
        int m = digits.mid(1, 2).toInt();
        //if (m > 59) m = 59;
        setText(QString("%1:%2").arg(h).arg(m, 2, 10, QChar('0')));
        setCursorPosition(text().length());
    }
    else if (digits.length() == 4) {
        int h = digits.left(2).toInt();
        int m = digits.right(2).toInt();
        //if (h > 23) h = 23;
        //if (m > 59) m = 59;
        setText(QString("%1:%2").arg(h,2,10,QChar('0')).arg(m,2,10,QChar('0')));
        setCursorPosition(text().length());
    }
}
