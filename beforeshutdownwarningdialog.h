#ifndef BEFORESHUTDOWNWARNINGDIALOG_H
#define BEFORESHUTDOWNWARNINGDIALOG_H

#include <QDialog>

#include "sstream"

namespace Ui {
class BeforeShutdownWarningDialog;
}

class BeforeShutdownWarningDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BeforeShutdownWarningDialog(int minutesBeforeShutdown, bool* acknowledge, QWidget *parent = nullptr);
    ~BeforeShutdownWarningDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::BeforeShutdownWarningDialog *ui;

    bool* acknowledgeReturn;
};

#endif // BEFORESHUTDOWNWARNINGDIALOG_H
